#include "musee.h"

static void usage(const char *prgname) {
  fprintf(stderr, "usage: %s\n", prgname);
}

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc == 1, "Ne prend aucun argument.");

  struct musee *m = get_musee(get_shm(), SHM_RDONLY);
  int semid = get_sem();

  // Waiting until the first semaphore goes to zero
  SEMOPS(semid, {SEM_CLOSED, 0, 0});
  INFOF("Le musée est ouvert ! (capacité: %d, file: %d)", m->capacite, m->file);
  set_sem_value(semid, SEM_SLEEP, 0);
  DEBUG("Marque en cours de traitement");

  short last_inside = 0, inside = 0;
  bool last_closed = true, closed = false;
  while (!(closed) || inside > 0) {
    SEMOPS(semid, {SEM_SLEEP, 0, 0}, {SEM_SLEEP, 1, 0});
    closed = (get_sem_value(semid, SEM_CLOSED) != 0);
    inside = (short)get_sem_value(semid, SEM_INSIDE);
    DEBUGF("Tick. %d personnes dans le musée.", inside);

    if (last_closed && !closed) {
      DEBUGF("Le musée vient d'ouvrir, enregistrement de la capacité %d",
             m->capacite - inside);

      if (inside != 0) {
        WARNF("Le musée ouvre alors qu'il y a déjà %d visiteurs dedans",
              inside);
      }

      set_sem_value(semid, SEM_CAPACITY, m->capacite - last_inside);
    } else if (!last_closed && closed) {
      DEBUG("Le musée vient de fermer, suppression de la capacité");
      set_sem_value(semid, SEM_CAPACITY, 0);
    }

    if (last_inside > inside) {
      INFOF("%d personnes viennent de sortir", last_inside - inside);
      if (!closed) {
        SEMOPS(semid, {SEM_CAPACITY, last_inside - inside, 0});
      }
    }

    last_inside = inside;
    last_closed = closed;
  }

  INFO("Le musée est fermé, et il n'y a plus de visiteurs.");

  return 0;
}

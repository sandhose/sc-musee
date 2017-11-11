#include "musee.h"

static void usage(const char *prgname) {
  fprintf(stderr, "usage: %s\n", prgname);
}

static int get_sem_val(int semid, unsigned short which) {
  int val;
  TRY_SYS(val = semctl(semid, which, GETVAL), "semctl");
  return val;
}

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc == 1, "Ne prend aucun argument.");

  struct musee *m = get_musee(get_shm());
  int semid = get_sem();

  // Waiting until the first semaphore goes to zero
  SEMOPS(semid, {SEM_CLOSED, 0, 0});
  INFOF("Le musée est ouvert ! (capacité: %d, file: %d)", m->capacite, m->file);
  TRY_SYS(semctl(semid, SEM_SLEEP, SETVAL, 0), "semctl");
  DEBUG("Marque en cours de traitement");

  short last_inside = 0, inside = 0;
  bool last_closed = true, closed = false;
  while (!(closed) || inside > 0) {
    SEMOPS(semid, {SEM_SLEEP, 0, 0}, {SEM_SLEEP, 1, 0});
    closed = get_sem_val(semid, SEM_CLOSED) != 0;
    inside = (short)get_sem_val(semid, SEM_INSIDE);
    DEBUGF("Tick. %d personnes dans le musée.", inside);

    if (last_closed && !closed) {
      TRY_SYS(semctl(semid, SEM_CAPACITY, SETVAL, m->capacite - last_inside),
              "semctl");
      DEBUGF("Le musée vient d'ouvrir, enregistrement de la capacité %d",
             m->capacite - inside);
    } else if (!last_closed && closed) {
      TRY_SYS(semctl(semid, SEM_CAPACITY, SETVAL, 0), "semctl");
      DEBUG("Le musée vient de fermer, suppression de la capacité");
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

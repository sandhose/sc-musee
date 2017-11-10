#include "musee.h"

static int get_sem_val(int, unsigned short);

static void usage(const char *prgname) {
  fprintf(stderr, "usage: %s\n", prgname);
}

int get_sem_val(int semid, unsigned short which) {
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
  TRY_SYS(semctl(semid, SEM_SLEEP, SETVAL, 1), "semctl");
  DEBUG("Marque en attente de travail");
  TRY_SYS(semctl(semid, SEM_CAPACITY, SETVAL, m->capacite), "semctl");
  DEBUG("Enregistre la capacité initiale");

  short last_inside = 0, inside;
  while (get_sem_val(semid, SEM_CLOSED) == 0 || last_inside > 0) {
    inside = (short)get_sem_val(semid, SEM_INSIDE);
    DEBUGF("Tick. %d personnes dans le musée.", inside);
    if (last_inside > inside) {
      INFOF("%d personnes viennent de sortir", last_inside - inside);
      SEMOPS(semid, {SEM_CAPACITY, last_inside - inside, 0});
    }
    last_inside = inside;

    SEMOPS(semid, {SEM_SLEEP, 0, 0}, {SEM_SLEEP, 1, 0});
  }

  return 0;
}

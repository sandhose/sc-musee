#include "musee.h"

static int get_sem_val(int, unsigned short);

void usage(const char *prgname) { fprintf(stderr, "usage: %s\n", prgname); }

int get_sem_val(int semid, unsigned short which) {
  int val;
  TRY_SYS(val = semctl(semid, which, GETVAL), "semctl");
  return val;
}

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc == 1, "Ne prend aucun argument.");

  struct musee *m = get_musee(get_shm());
  int semid = get_sem();
  printf("Capacité: %d, file: %d, semid: %d\n", m->capacite, m->file, semid);

  // Waiting until the first semaphore goes to zero
  SEMOPS(semid, {SEM_CLOSED, 0, 0});
  printf("Le musée est ouvert !\n");
  TRY_SYS(semctl(semid, SEM_SLEEP, SETVAL, 1), "semctl");
  TRY_SYS(semctl(semid, SEM_CAPACITY, SETVAL, m->capacite), "semctl");

  short last_inside = 0, inside;
  while (get_sem_val(semid, SEM_CLOSED) == 0) {
    printf("Tick.\n");

    inside = (short)get_sem_val(semid, SEM_INSIDE);
    if (last_inside > inside) {
      printf("Adding %d capacity\n", last_inside - inside);
      SEMOPS(semid, {SEM_CAPACITY, last_inside - inside, 0});
    }
    last_inside = inside;

    SEMOPS(semid, {SEM_SLEEP, 0, 0}, {SEM_SLEEP, 1, 0});
  }

  return 0;
}

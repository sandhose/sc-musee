#include "musee.h"

void usage(const char *prgname) { fprintf(stderr, "usage: %s\n", prgname); }

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc == 1, "Ne prend aucun argument.");

  struct musee *m = get_musee(get_shm());
  int semid = get_sem();
  printf("Capacité: %d, file: %d, semid: %d\n", m->capacite, m->file, semid);

  // Waiting until the first semaphore goes to zero
  struct sembuf s[1] = {{0, 0, 0}};
  TRY(semop(semid, s, 1) != -1, "semop");
  printf("Le musée est ouvert !\n");

  return 0;
}

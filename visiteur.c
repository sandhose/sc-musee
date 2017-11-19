#include "musee.h"
#include <time.h>

static void usage(const char *prgname) {
  fprintf(stderr, "usage: %s <durée>\n", prgname);
}

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc == 2, "Requiert un argument.");

  char *endptr = NULL;
  long sleep_ms = strtol(argv[1], &endptr, 10);
  TRY_OR_USAGE(endptr != argv[1] && *endptr == '\0' && sleep_ms > 0,
               "La durée doit être un nombre strictement positif.");
  struct timespec t = {sleep_ms / 1000, (sleep_ms % 1000) * 1000};

  struct musee *m = get_musee(get_shm(), SHM_RDONLY);
  int semid = get_sem();

  int waiting;
  TRY_SYS(waiting = semctl(semid, SEM_CAPACITY, GETNCNT), "semctl");
  INFOF("%d personnes sont en attente.", waiting);
  if (waiting >= m->file) {
    ERROR("Trop de personnes attendent, je ne rentre pas dans la file.");
    return EXIT_FAILURE;
  }
  SEMOPS(semid, {SEM_CAPACITY, -1, 0}, {SEM_INSIDE, 1, 0}, {SEM_SLEEP, -1, 0});
  INFO("Je suis dans le musée");
  nanosleep(&t, NULL);

  struct sembuf s[2] = {{SEM_INSIDE, -1, IPC_NOWAIT}, {SEM_SLEEP, -1, 0}};
  int ret;
  TRY_SYS(ret = semop(semid, s, 2), "semop");
  if (ret == EAGAIN) {
    FATAL("Je n'ai pas réussi à sortir.");
    return EXIT_FAILURE;
  } else {
    INFO("Je suis sorti du musée.");
  }
}

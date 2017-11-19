#include "musee.h"
#include <time.h>

static void usage(const char *prgname) {
  fprintf(stderr, "usage: %s <durée>\n", prgname);
}

int main(int argc, char *argv[]) {
  ASSERT_OR_USAGE(argc == 2, "Requiert un argument.");

  // Parse le temps en argument, et construit la structure timespec
  char *endptr = NULL;
  long sleep_ms = strtol(argv[1], &endptr, 10);
  ASSERT_OR_USAGE(endptr != argv[1] && *endptr == '\0' && sleep_ms > 0,
                  "La durée doit être un nombre strictement positif.");
  struct timespec t = {sleep_ms / 1000, (sleep_ms % 1000) * 1000};

  struct musee *m = get_musee(get_shm(), true);
  int semid = get_sem();

  // Vérifie le nombre de personnes en attente
  int waiting;
  ASSERT_SYS(waiting = semctl(semid, SEM_CAPACITY, GETNCNT), "semctl");
  INFOF("%d personnes sont en attente.", waiting);
  if (waiting >= m->file) {
    ERROR("Trop de personnes attendent, je ne rentre pas dans la file.");
    return EXIT_FAILURE;
  }

  // Tente de rentrer, d'ajouter 1 au nombre de personnes à l'intérieur, et
  // réveiller le contrôleur
  SEMOPS(semid, {SEM_CAPACITY, -1, 0}, {SEM_INSIDE, 1, 0}, {SEM_SLEEP, -1, 0});
  INFO("Je suis dans le musée");
  nanosleep(&t, NULL);

  // Tente de sortir du musée, en réveillant le contrôleur (IPC_NOWAIT pour
  // éviter d'éventuels hard locks)
  SEMOPS(semid, {SEM_INSIDE, -1, IPC_NOWAIT}, {SEM_SLEEP, -1, 0});
  INFO("Je suis sorti du musée.");
}

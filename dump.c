// Print everything regardless of the environment variable
#define LOG_LEVEL 5

#include "musee.h"

static void usage(const char *prgname) {
  fprintf(stderr, "usage: %s\n", prgname);
}

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc == 1, "Ne prend aucun argument.");
  struct musee *m = get_musee(get_shm());
  int semid = get_sem();

  int waiting, inside, closed, cur_capacite;
  TRY_SYS(waiting = semctl(semid, SEM_CAPACITY, GETNCNT), "semctl");
  TRY_SYS(cur_capacite = semctl(semid, SEM_CAPACITY, GETVAL), "semctl");
  TRY_SYS(inside = semctl(semid, SEM_INSIDE, GETVAL), "semctl");
  TRY_SYS(closed = semctl(semid, SEM_CLOSED, GETVAL), "semctl");

  INFOF("  Capacité max: %d", m->capacite);
  INFOF("          File: %d", m->file);
  INFOF("          État: %s", closed ? "fermé" : "ouvert");
  INFOF("Capa. actuelle: %d", cur_capacite);
  INFOF("    En attente: %d", waiting);
  INFOF(" À l'intérieur: %d", inside);
}

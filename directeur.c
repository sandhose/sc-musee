#include <string.h>

#include "musee.h"

static int creer(int, int);
static int ouvrir(void);
static int fermer(void);
static int supprimer(void);

void usage(const char *prgname) {
  fprintf(stderr,
          "usage: %s <command>\n"
          "Commande disponibles:\n"
          "  creer <capacité> <file>  foo\n"
          "  ouvrir                   foo\n"
          "  fermer                   foo\n"
          "  supprimer                foo\n",
          prgname);
}

int main(int argc, char *argv[]) {
  TRY_OR_USAGE(argc > 1, "Il faut au moins un argument");

  char *subcommand = argv[1];
  if (strcmp(subcommand, "creer") == 0) {
    TRY_OR_USAGE(argc == 4,
                 "`creer' a besoin de deux arguments supplémentaires.");

    char *endptr = NULL;
    long capacite = strtol(argv[2], &endptr, 10);
    TRY_OR_USAGE(endptr != argv[2] && *endptr == '\0' && capacite > 0,
                 "La capacité doit être un nombre strictement positif.");

    long file = strtol(argv[3], &endptr, 10);
    TRY_OR_USAGE(endptr != argv[2] && *endptr == '\0' && file > 0,
                 "La file doit être un nombre strictement positif.");

    return creer((int)capacite, (int)file);
  } else if (strcmp(subcommand, "ouvrir") == 0) {
    TRY_OR_USAGE(argc == 2, "`ouvrir' ne prend pas d'argument.");
    return ouvrir();
  } else if (strcmp(subcommand, "fermer") == 0) {
    TRY_OR_USAGE(argc == 2, "`fermer' ne prend pas d'argument.");
    return fermer();
  } else if (strcmp(subcommand, "supprimer") == 0) {
    TRY_OR_USAGE(argc == 2, "`supprimer' ne prend pas d'argument.");
    return supprimer();
  } else {
    printf("Commande inconnue `%s'\n", subcommand);
    usage(argv[0]);
    return EXIT_FAILURE;
  }
}

int creer(int capacite, int file) {
  int shmid = create_shm();
  int semid = create_sem();
  struct musee *m = get_musee(shmid);
  m->capacite = capacite;
  m->file = file;
  printf("Capacité: %d, file: %d, shmid: %d, semid: %d\n", capacite, file,
         shmid, semid);
  return EXIT_SUCCESS;
}

int ouvrir(void) {
  int semid = get_sem();
  TRY(semctl(semid, 0, SETVAL, 0) != -1, "semctl");
  return EXIT_SUCCESS;
}

int fermer(void) {
  int semid = get_sem();
  TRY(semctl(semid, 0, SETVAL, 1) != -1, "semctl");
  return EXIT_SUCCESS;
}

int supprimer(void) {
  delete_shm();
  delete_sem();
  return EXIT_SUCCESS;
}

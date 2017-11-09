#include <string.h>

#include "musee.h"

static int creer(int, int);
static int ouvrir(void);
static int fermer(void);
static int supprimer(void);
static int tick(void);

static void usage(const char *prgname) {
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
  DEBUG("Interprétation des arguments.");
  TRY_OR_USAGE(argc > 1, "Il faut au moins un argument.");

  char *subcommand = argv[1];
  if (strcmp(subcommand, "creer") == 0) {
    DEBUG("Sous-commande `creer'")
    TRY_OR_USAGE(argc == 4,
                 "`creer' a besoin de deux arguments supplémentaires.");

    char *endptr = NULL;
    long capacite = strtol(argv[2], &endptr, 10);
    TRY_OR_USAGE(endptr != argv[2] && *endptr == '\0' && capacite > 0,
                 "La capacité doit être un nombre strictement positif.");
    DEBUGF("Capacité = %lu", capacite);

    long file = strtol(argv[3], &endptr, 10);
    TRY_OR_USAGE(endptr != argv[2] && *endptr == '\0' && file > 0,
                 "La file doit être un nombre strictement positif.");
    DEBUGF("File = %lu", file);

    return creer((int)capacite, (int)file);
  } else if (strcmp(subcommand, "ouvrir") == 0) {
    DEBUG("Sous-commande `ouvrir'")
    TRY_OR_USAGE(argc == 2, "`ouvrir' ne prend pas d'argument.");
    return ouvrir();
  } else if (strcmp(subcommand, "fermer") == 0) {
    DEBUG("Sous-commande `fermer'")
    TRY_OR_USAGE(argc == 2, "`fermer' ne prend pas d'argument.");
    return fermer();
  } else if (strcmp(subcommand, "tick") == 0) {
    DEBUG("Sous-commande `tick'")
    TRY_OR_USAGE(argc == 2, "`tick' ne prend pas d'argument.");
    return tick();
  } else if (strcmp(subcommand, "supprimer") == 0) {
    DEBUG("Sous-commande `supprimer'")
    TRY_OR_USAGE(argc == 2, "`supprimer' ne prend pas d'argument.");
    return supprimer();
  } else {
    printf("Commande inconnue `%s'\n", subcommand);
    usage(argv[0]);
    return EXIT_FAILURE;
  }
}

int creer(int capacite, int file) {
  INFO("Création de la mémoire partagée…");
  int shmid = create_shm();
  INFO("Création de l'ensemble de sémaphores…");
  (void)create_sem();
  struct musee *m = get_musee(shmid);
  m->capacite = capacite;
  m->file = file;
  INFOF("Un musée de capacité %d et de file %d a été créé.", capacite, file);
  return EXIT_SUCCESS;
}

int ouvrir(void) {
  INFO("Ouverture du musée…");
  int semid = get_sem();
  TRY_SYS(semctl(semid, SEM_CLOSED, SETVAL, 0), "semctl");
  INFO("Le musée est ouvert.");
  return EXIT_SUCCESS;
}

int tick(void) {
  int semid = get_sem();
  TRY_SYS(semctl(semid, SEM_SLEEP, SETVAL, 0), "semctl");
  return EXIT_SUCCESS;
}

int fermer(void) {
  INFO("Fermeture du musée…");
  int semid = get_sem();
  TRY_SYS(semctl(semid, SEM_CLOSED, SETVAL, 1), "semctl");
  TRY_SYS(semctl(semid, SEM_SLEEP, SETVAL, 0), "semctl");
  INFO("Le musée est fermé.");
  return EXIT_SUCCESS;
}

int supprimer(void) {
  delete_shm();
  delete_sem();
  return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "musee.h"

int creer(int, int);
int ouvrir(void);
int fermer(void);
int supprimer(void);

void usage(char *prgname) {
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
  TRY_OR_USAGE(argc < 2, "Il faut au moins un argument");

  char *subcommand = argv[1];
  if (strcmp(subcommand, "creer") == 0) {
    TRY_OR_USAGE(argc != 4,
                 "`creer' a besoin de deux arguments supplémentaires.");

    char *endptr = NULL;
    long capacite = strtol(argv[2], &endptr, 10);
    TRY_OR_USAGE(endptr == argv[2] || *endptr != '\0' || capacite < 1,
                 "La capacité doit être un nombre strictement positif.");

    long file = strtol(argv[3], &endptr, 10);
    TRY_OR_USAGE(endptr == argv[2] || *endptr != '\0' || file < 1,
                 "La file doit être un nombre strictement positif.");

    return creer((int)capacite, (int)file);
  } else if (strcmp(subcommand, "ouvrir") == 0) {
    TRY_OR_USAGE(argc != 2, "`ouvrir' ne prend pas d'argument.");
    return ouvrir();
  } else if (strcmp(subcommand, "fermer") == 0) {
    TRY_OR_USAGE(argc != 2, "`fermer' ne prend pas d'argument.");
    return fermer();
  } else if (strcmp(subcommand, "supprimer") == 0) {
    TRY_OR_USAGE(argc != 2, "`supprimer' ne prend pas d'argument.");
    return supprimer();
  } else {
    printf("Commande inconnue `%s'\n", subcommand);
    usage(argv[0]);
    return EXIT_FAILURE;
  }
}

int creer(int capacite, int file) {
  printf("Capacité: %d, file: %d\n", capacite, file);
  return EXIT_SUCCESS;
}

int ouvrir(void) { return EXIT_SUCCESS; }
int fermer(void) { return EXIT_SUCCESS; }
int supprimer(void) { return EXIT_SUCCESS; }

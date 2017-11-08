#ifndef MUSEE_H
#define MUSEE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>

#define TRY(EXPR, MSG)                                                         \
  {                                                                            \
    if (!(EXPR)) {                                                             \
      fprintf(stderr, "%s:%d ", __FILE__, __LINE__);                           \
      perror(MSG);                                                             \
      exit(1);                                                                 \
    }                                                                          \
  }

#define TRY_OR_USAGE(CHK, MSG)                                                 \
  {                                                                            \
    if (CHK) {                                                                 \
      usage(argv[0]);                                                          \
      fprintf(stderr, "\n" MSG "\n");                                          \
      return EXIT_FAILURE;                                                     \
    }                                                                          \
  }

// This functions has to be defined in each program to use TRY_OR_USAGE
void usage(char *);

struct musee {
  int capacite;
  int file;
  bool ouvert;
};

#endif

#ifndef MUSEE_H
#define MUSEE_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

// Double expansion technique to convert __LINE__ into string literal
#define S(x) #x
#define S_(x) S(x)
#define TRY(EXPR, MSG)                                                         \
  {                                                                            \
    if (!(EXPR)) {                                                             \
      perror(__FILE__ ":" S_(__LINE__) " " MSG);                               \
      exit(1);                                                                 \
    }                                                                          \
  }

#define TRY_OR_USAGE(CHK, MSG)                                                 \
  {                                                                            \
    if (!(CHK)) {                                                              \
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

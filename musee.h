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
void usage(const char *);

key_t token(void);
int create_shm(void);
void delete_shm(void);
int get_shm(void);

int create_sem(void);
void delete_sem(void);
int get_sem(void);

struct musee *get_musee(int);

struct musee {
  int capacite;
  int file;
};

#endif

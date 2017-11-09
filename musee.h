#ifndef MUSEE_H
#define MUSEE_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define SEM_CLOSED 0
#define SEM_SLEEP 1
#define SEM_CAPACITY 2
#define SEM_INSIDE 3

// Run multiple ops on semaphores. SEMOPS(semid, {id, 0, 0}, {id, 1, 0})
#define SEMOPS(SEMID, ...)                                                     \
  {                                                                            \
    struct sembuf s[] = {__VA_ARGS__};                                         \
    TRY_SYS(semop(SEMID, s, sizeof(s) / sizeof(struct sembuf)), "semop");      \
  }

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
#define TRY_SYS(EXPR, MSG) TRY((int)(EXPR) != -1, MSG)

#define TRY_OR_USAGE(CHK, MSG)                                                 \
  {                                                                            \
    if (!(CHK)) {                                                              \
      usage(argv[0]);                                                          \
      fprintf(stderr, "\n" MSG "\n");                                          \
      exit(1);                                                                 \
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

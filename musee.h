#ifndef MUSEE_H
#define MUSEE_H

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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

#define TRY(EXPR, MSG)                                                         \
  {                                                                            \
    if (!(EXPR)) {                                                             \
      perror(LOG_FMT(FATAL, MSG));                                             \
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

#ifndef LOG_LEVEL
#define LOG_LEVEL get_log_level()
#endif

#define LEVEL_FATAL 0
#define COLOR_FATAL "\x1b[1;35;7m" // Magenta
#define LEVEL_ERROR 1
#define COLOR_ERROR "\x1b[1;31;7m" // Red
#define LEVEL_WARN 1
#define COLOR_WARN "\x1b[1;33;7m" // Yellow
#define LEVEL_INFO 1
#define COLOR_INFO "\x1b[1;32;7m" // Green
#define LEVEL_DEBUG 2
#define COLOR_DEBUG "\x1b[1;36;7m" // Cyan

#define FATAL(MSG) LOG(FATAL, MSG "\n")
#define FATALF(MSG, ...) LOG(FATAL, MSG "\n", __VA_ARGS__)
#define ERROR(MSG) LOG(ERROR, MSG "\n")
#define ERRORF(MSG, ...) LOG(ERROR, MSG "\n", __VA_ARGS__)
#define WARN(MSG) LOG(WARN, MSG "\n")
#define WARNF(MSG, ...) LOG(WARN, MSG "\n", __VA_ARGS__)
#define INFO(MSG) LOG(INFO, MSG "\n")
#define INFOF(MSG, ...) LOG(INFO, MSG "\n", __VA_ARGS__)
#define DEBUG(MSG) LOG(DEBUG, MSG "\n")
#define DEBUGF(MSG, ...) LOG(DEBUG, MSG "\n", __VA_ARGS__)

// Double expansion technique to convert __LINE__ into string literal
#define S(x) #x
#define S_(x) S(x)

#if NO_COLOR
#define LEVEL_FMT(LEVEL) #LEVEL
#define LOC_FMT __FILE__ ":" S_(__LINE__)
#else
#define LEVEL_FMT(LEVEL) COLOR_##LEVEL " " #LEVEL " \x1b[0m"
#define LOC_FMT "\x1b[90m" __FILE__ ":" S_(__LINE__) "\x1b[0m"
#endif

#define LOG_FMT(LEVEL, ...) LEVEL_FMT(LEVEL) "\t" LOC_FMT "\t" __VA_ARGS__
#define LOG(LEVEL, ...)                                                        \
  {                                                                            \
    if (LEVEL_##LEVEL <= LOG_LEVEL) {                                          \
      fprintf(stderr, LOG_FMT(LEVEL, __VA_ARGS__));                            \
      fflush(stderr);                                                          \
    }                                                                          \
  }

int get_log_level(void);

key_t token(void);
int create_shm(void);
void delete_shm(void);
int get_shm(void);

int create_sem(void);
void delete_sem(void);
int get_sem(void);
int get_sem_value(int semid, unsigned short which);
void set_sem_value(int semid, unsigned short which, unsigned short value);

struct musee *get_musee(int shmid, int shmflg);

struct musee {
  int capacite;
  int file;
};

#endif

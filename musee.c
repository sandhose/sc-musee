#include "musee.h"

key_t token(void) {
  key_t k;
  TRY_SYS(k = ftok("/etc/passwd", 0), "ftok");
  return k;
}

int create_shm(void) {
  key_t k = token();
  int shmid = shmget(k, 0, 0);
  if (shmid > -1) {
    fprintf(stderr, "Un musée existe déjà.\n");
    exit(EXIT_FAILURE);
  }

  TRY(errno == ENOENT, "shmget");

  TRY_SYS(shmid = shmget(k, sizeof(struct musee), IPC_CREAT | 0666), "shmget");
  return shmid;
}

int get_shm(void) {
  key_t k = token();
  int shmid;
  TRY_SYS(shmid = shmget(k, 0, 0), "shmget");
  return shmid;
}

void delete_shm(void) {
  int shmid = get_shm();
  TRY_SYS(shmctl(shmid, IPC_RMID, NULL), "shmctl");
}

struct musee *get_musee(int shmid) {
  void *m;
  TRY_SYS(m = shmat(shmid, NULL, 0), "shmat");
  return (struct musee *)m;
}

int create_sem(void) {
  key_t k = token();
  int semid = semget(k, 0, 0);
  if (semid > -1) {
    fprintf(stderr, "La sémaphore existe déjà.\n");
    exit(EXIT_FAILURE);
  }

  TRY(errno == ENOENT, "semget");

  TRY_SYS(semid = semget(k, 4, IPC_CREAT | 0666), "semget");
  TRY_SYS(semctl(semid, 0, SETVAL, 1), "semctl"); // 1 = Closed
  return semid;
}

int get_sem(void) {
  key_t k = token();
  int semid;
  TRY_SYS(semid = semget(k, 0, 0), "semget");
  return semid;
}

void delete_sem(void) {
  int semid = get_sem();
  TRY_SYS(semctl(semid, 0, IPC_RMID), "semctl");
}

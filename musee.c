#include "musee.h"

key_t token(void) {
  key_t k = ftok("/etc/passwd", 0);
  TRY(k != -1, "ftok");
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

  TRY((shmid = shmget(k, sizeof(struct musee), IPC_CREAT | 0666)) != -1,
      "shmget");
  return shmid;
}

int get_shm(void) {
  key_t k = token();
  int shmid;
  TRY((shmid = shmget(k, 0, 0)) != -1, "shmget");
  return shmid;
}

void delete_shm(void) {
  int shmid = get_shm();
  TRY(shmctl(shmid, IPC_RMID, NULL) != -1, "shmctl");
}

struct musee *get_musee(int shmid) {
  void *m;
  TRY((m = shmat(shmid, NULL, 0)) != (void *)-1, "shmat");
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

  TRY((semid = semget(k, 1, IPC_CREAT | 0666)) != -1, "semget");
  TRY(semctl(semid, 0, SETVAL, 1) != -1, "semctl"); // 1 = Closed
  return semid;
}

int get_sem(void) {
  key_t k = token();
  int semid;
  TRY((semid = semget(k, 0, 0)) != -1, "semget");
  return semid;
}

void delete_sem(void) {
  int semid = get_sem();
  TRY(semctl(semid, 0, IPC_RMID) != -1, "semctl");
}

#include "musee.h"

int get_log_level(void) {
  char *env = getenv("DEBUG_MUSEE");
  if (env == NULL)
    return 0;
  int level = atoi(env);
  return level >= 0 ? level : 0;
}

key_t token(void) {
  key_t k;
  ASSERT_SYS(k = ftok("/etc/passwd", 0), "ftok");
  return k;
}

int create_shm(void) {
  key_t k = token();
  int shmid = shmget(k, 0, 0);
  if (shmid > -1) {
    WARN("Un espace de mémoire partagé existe déjà, "
         "suppression de l'ancien espace.");
    ASSERT_SYS(shmctl(shmid, IPC_RMID, NULL), "shmctl");
  } else {
    ASSERT(errno == ENOENT, "shmget");
  }

  ASSERT_SYS(shmid = shmget(k, sizeof(struct musee), IPC_CREAT | 0666),
             "shmget");
  DEBUGF("shmid = %d", shmid);
  return shmid;
}

int get_shm(void) {
  key_t k = token();
  int shmid;
  ASSERT_SYS(shmid = shmget(k, 0, 0), "shmget");
  DEBUGF("shmid = %d", shmid);
  return shmid;
}

void delete_shm(void) {
  int shmid = get_shm();
  ASSERT_SYS(shmctl(shmid, IPC_RMID, NULL), "shmctl");
}

struct musee *get_musee(int shmid, bool readonly) {
  int shmflg = readonly ? SHM_RDONLY : 0;
  void *m;
  ASSERT((m = shmat(shmid, NULL, shmflg)) != (void *)-1, "shmat");
  return (struct musee *)m;
}

int create_sem(void) {
  key_t k = token();
  int semid = semget(k, 0, 0);
  if (semid > -1) {
    WARN("Un ensemble de sémaphores existe déjà, "
         "supression de l'ancien ensemble.")
    ASSERT_SYS(semctl(semid, 0, IPC_RMID), "semctl");
  } else {
    ASSERT(errno == ENOENT, "semget");
  }

  ASSERT_SYS(semid = semget(k, 4, IPC_CREAT | 0666), "semget");
  ASSERT_SYS(semctl(semid, 0, SETVAL, 1), "semctl"); // 1 = Closed
  DEBUGF("semid = %d", semid);
  return semid;
}

int get_sem(void) {
  key_t k = token();
  int semid;
  ASSERT_SYS(semid = semget(k, 0, 0), "semget");
  DEBUGF("semid = %d", semid);
  return semid;
}

void delete_sem(void) {
  int semid = get_sem();
  ASSERT_SYS(semctl(semid, 0, IPC_RMID), "semctl");
}

int get_sem_value(int semid, unsigned short which) {
  int val;
  ASSERT_SYS(val = semctl(semid, which, GETVAL), "semctl");
  return val;
}

void set_sem_value(int semid, unsigned short which, unsigned short value) {
  ASSERT_SYS(semctl(semid, which, SETVAL, value), "semctl");
}

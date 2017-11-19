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

// Wrapper autour de semop, qui construit la structure à la volée, et vérifie
// le code de retour. Ex: SEMOPS(semid, {SEM_SLEEP, -1, 0}, {SEM_INSIDE, 1, 0})
#define SEMOPS(SEMID, ...)                                                     \
  {                                                                            \
    struct sembuf s[] = {__VA_ARGS__};                                         \
    ASSERT_SYS(semop(SEMID, s, sizeof(s) / sizeof(struct sembuf)), "semop");   \
  }

// Macro qui vérifie une expression EXPR, et l'erreur avec un message si elle
// est fausse + exit.
#define ASSERT(EXPR, MSG)                                                      \
  {                                                                            \
    if (!(EXPR)) {                                                             \
      perror(LOG_FMT(FATAL, MSG));                                             \
      exit(1);                                                                 \
    }                                                                          \
  }

// Similaire à ASSERT, mais compare directement à -1 (pour les primitives
// système)
#define ASSERT_SYS(EXPR, MSG) ASSERT((int)(EXPR) != -1, MSG)

// Similaire à ASSERT, mais affiche le message d'usage (sans perror)
#define ASSERT_OR_USAGE(CHK, MSG)                                              \
  {                                                                            \
    if (!(CHK)) {                                                              \
      usage(argv[0]);                                                          \
      fprintf(stderr, "\n" MSG "\n");                                          \
      exit(1);                                                                 \
    }                                                                          \
  }

// Permet de définir LOG_LEVEL à une constante, à la compilation ou par
// programme. C'est le cas de dump, qui a son niveau de log au maximum, quoi
// qu'il arrive.
#ifndef LOG_LEVEL
#define LOG_LEVEL get_log_level()
#endif

// "Niveau" de chaque type de log. Au début, c'était de 0 à 4, mais le test qui
// vérifiait les niveaux de logs ne passait pas avec ça.
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

// Macros pour logger.
// À chaque fois, une variante avec et sans formattage à la printf.
// J'ai dû séparer pour pas avoir à traîner des retours à la ligne, et quand
// même faire un seul appel à fprintf.
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

// Compiler avec `-DNO_COLOR` pour desactiver les couleurs.
// TODO: Utiliser isatty afficher intelligemment des couleurs.
#ifdef NO_COLOR
#define LEVEL_FMT(LEVEL) #LEVEL
#define LOC_FMT __FILE__ ":" S_(__LINE__)
#else
#define LEVEL_FMT(LEVEL) COLOR_##LEVEL " " #LEVEL " \x1b[0m"
#define LOC_FMT "\x1b[90m" __FILE__ ":" S_(__LINE__) "\x1b[0m"
#endif

// Macro interne pour formatter un message de logs (en couleur), avec
// [niveau] [fichier]:[ligne] [message]
#define LOG_FMT(LEVEL, ...) LEVEL_FMT(LEVEL) "\t" LOC_FMT "\t" __VA_ARGS__
#define LOG(LEVEL, ...)                                                        \
  {                                                                            \
    if (LEVEL_##LEVEL <= LOG_LEVEL) {                                          \
      fprintf(stderr, LOG_FMT(LEVEL, __VA_ARGS__));                            \
      fflush(stderr);                                                          \
    }                                                                          \
  }

// Récupère le niveau de logs dans la variable d'environnement DEBUG_MUSEE
int get_log_level(void);

// Créé une clé à utiliser pour le shm et les sem.
key_t token(void);

// -- Mémoire partagée --
// Créer un shm, en supprimant l'ancien s'il existe
int create_shm(void);

// Supprime un shm
void delete_shm(void);

// Récupère l'ID du shm
int get_shm(void);

// Récupère le musée (éventuellement en lecture seule)
struct musee *get_musee(int shmid, bool readonly);

struct musee {
  int capacite;
  int file;
};

// -- Ensemble de sémaphores --
// Créé l'ensemble de sémaphores, en supprimant l'ancien s'il existe
int create_sem(void);

// Supprime l'ensemble de sémaphores
void delete_sem(void);

// Récupère l'ID de l'ensemble de sémaphores
int get_sem(void);

// Récupère la valeur d'un des sémaphores
int get_sem_value(int semid, unsigned short which);

// Change la valeur d'un des sémaphores
void set_sem_value(int semid, unsigned short which, unsigned short value);

#endif

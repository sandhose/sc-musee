#ifndef MUSEE_H
#define MUSEE_H

#define CHECK_OR_USAGE(CHK, MSG)                                               \
  {                                                                            \
    if (CHK) {                                                                 \
      usage(argv[1]);                                                          \
      fprintf(stderr, "\n" MSG "\n");                                          \
      return EXIT_FAILURE;                                                     \
    }                                                                          \
  }

// This functions has to be defined in each program to use CHECK_OR_USAGE
void usage(char *);

struct musee {
  int capacite;
  int file;
  bool ouvert;
};

#endif

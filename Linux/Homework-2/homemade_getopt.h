#ifndef HOMEMADE_GETOPT_H
#define HOMEMADE_GETOPT_H

#ifndef _HOMEMADE_SRC
#include <getopt.h>
#else

#if defined(__cplusplus)
extern "C" {
#endif


#define no_argument 1
#define required_argument 2
#define optional_argument 3

extern char* optarg;
extern int optind;

struct option {
  const char* name;
  int has_arg;
  int* flag;
  int val;
};

int homemade_getopt(int argc, char* const argv[], const char* optstring);
#if defined(__cplusplus)
}
#endif

#define getopt homemade_getopt

#endif

#endif // HOMEMADE_GETOPT_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define _HOMEMADE_SRC
#include "homemade_getopt.h"

// Global definitions
char *optarg;
int optind = 1;

static char *optcursor;

int homemade_getopt(int argc, char* const argv[], const char* optstring) {
    int optchar = EOF;

    const char *optdecl = NULL;

    // Initialize global vars
    optarg = NULL;

    // Arguments overflow #1
    if(optind >= argc)
        goto no_more_argument;

    // Arguments overflow #2
    if(argv[optind] == NULL)
        goto no_more_argument;
    
    // Invalid arguments #1
    if(argv[optind][0] != '-')
        goto no_more_argument;
    
    // Invalid arguments #2
    if(strcmp(argv[optind], "-") == 0)
        goto no_more_argument;
    
    // Long arguments #1
    // Skip it here
    if(strcmp(argv[optind], "--") == 0) {
        optind++;
        goto no_more_argument;   
    }

    if (optcursor == NULL || optcursor[0] == '\0')
        optcursor = argv[optind] + 1;

    optchar = optcursor[0];

    optdecl = strchr(optstring, optchar);
    if (optdecl) {
        if (optdecl[1] == ':') {
            optarg = ++optcursor;
            if (*optarg == '\0') {
                if (++optind < argc) {
                    optarg = argv[optind];
                } else {
                    optarg = NULL;
                    optchar = (optstring[0] == ':') ? ':' : '?';
                }
            }
            optcursor = NULL;
        }
    } else {
        optchar = '?';
    }

    if (optcursor == NULL || *++optcursor == '\0')
        ++optind;

    return optchar;
no_more_argument:
    optcursor = NULL;
    return EOF;
}
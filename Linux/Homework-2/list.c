#define _HOMEMADE_SRC
#include "homemade_getopt.h"

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

// Global options
static const char *elf_name;
static int recursive_flag;
static int all_flag;
static off_t lo_size;
static off_t hi_size;
static time_t modify_time;
static int init = 1;

// Helper linked list
typedef struct lnode {
    const char *node;
    struct lnode *next;
} list_node_t;

typedef list_node_t *list_t;

static list_t list_start;
static list_t list_end;

static char get_type(mode_t mode) {
    if(S_ISREG(mode))
        return '-';
    if(S_ISDIR(mode))
        return 'd';
    if(S_ISCHR(mode))
        return 'c';
    if(S_ISBLK(mode))
        return 'b';
    if(S_ISLNK(mode))
        return 'l';
    if(S_ISFIFO(mode))
        return 'p';
    if(S_ISSOCK(mode))
        return 's';
}

static void list_node(const char *prefix, const char *file_name) {
    size_t name_size = strlen(prefix);
    size_t entry_size = strlen(file_name);
    char *node_name = malloc(name_size + entry_size + 2);

    strcpy(node_name, prefix);
    node_name[name_size] = '/';
    strcpy (&node_name[name_size + 1], file_name);
    node_name[name_size + 1 + entry_size] = 0;

    struct stat status;
    if(stat(node_name, &status)) {
        fprintf(stderr, "%s - Can't access \"%s\": %s\n", elf_name, node_name, strerror(errno));
        return;
    }

    mode_t stat_mode = status.st_mode;
    off_t stat_size = status.st_size;
    time_t stat_mtime = status.st_mtime;

    char mode_text[] = {'-', '-', '-', '-', '-',
                        '-', '-', '-', '-', '-', '\0'};
    mode_text[0] = get_type(stat_mode);
    if(stat_mode & S_IRUSR) {
        mode_text[1] = 'r';
    }
    if(stat_mode & S_IWUSR) {
        mode_text[2] = 'w';
    }
    if(stat_mode & S_IXUSR) {
        mode_text[3] = 'x';
    }
    if(stat_mode & S_IRGRP) {
        mode_text[4] = 'r';
    }
    if(stat_mode & S_IWGRP) {
        mode_text[5] = 'w';
    }
    if(stat_mode & S_IXGRP) {
        mode_text[6] = 'x';
    }
    if(stat_mode & S_IROTH) {
        mode_text[7] = 'r';
    }
    if(stat_mode & S_IWOTH) {
        mode_text[8] = 'w';
    }
    if(stat_mode & S_IXOTH) {
        mode_text[9] = 'x';
    }

    int filter_flag = (!modify_time || time (NULL) - stat_mtime <= modify_time) &&
        (!lo_size || stat_size >= lo_size) &&
        (!hi_size || stat_size <= hi_size);
    if((filter_flag && !S_ISDIR(stat_mode)) || (S_ISDIR(stat_mode) && recursive_flag)) {
        char stat_time_str[64];
        strftime(stat_time_str, 64, 
            "%Y-%m-%d %H:%M", localtime(&stat_mtime));
        printf ("%s %8ld %s %s\n", mode_text, stat_size, stat_time_str, file_name);
    }
    
    if(S_ISDIR(stat_mode) && recursive_flag) {
        list_t tmp = malloc(sizeof(list_node_t));
        tmp->node = node_name;
        tmp->next = NULL;
        list_end->next = tmp;
        list_end = tmp;
    }
}

static void list_dir(const char *name) {
    if(!init) {
        printf("\n");
    }
    init = 0;

    DIR *dir = opendir (name);
    if (dir == NULL) {
            fprintf(stderr, "%s - Can't access dir \"%s\": %s\n", elf_name, name, strerror(errno));
            return;
        }

    printf("%s:\n", name);

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        const char *entry_name = entry->d_name;
        if(!all_flag && entry_name[0] == '.')
            continue;
        
        list_node(name, entry_name);
        count++;
    }
    printf("%d files in total.\n", count);
}

static void list_main(const char *name) {
    struct stat status;
    if(stat(name, &status)) {
        fprintf(stderr, "%s - Can't access \"%s\": %s\n", elf_name, name, strerror(errno));
        return;
    }
    if(S_ISDIR(status.st_mode)) {
        list_start = malloc(sizeof(list_node_t));
        list_start->node = name;
        list_start->next = NULL;
        list_end = list_start;
        while (list_start) {
            list_dir(list_start->node);
            list_start = list_start->next;
        }
    } else {
        list_node("", name);
    }
}

int main(int argc, char **argv) {
    elf_name = argv[0];
    int c = 0;
    while ((c = getopt (argc, argv, "ral:h:m:")) != -1) {
        switch (c) {
            case 'r':
                recursive_flag = 1;
                break;
            case 'a':
                all_flag = 1;
                break;
            case 'l':
                lo_size = atoi(optarg);
                break;
            case 'h':
                hi_size = atoi(optarg);
                break;
            case 'm':
                modify_time = atoi (optarg) * 24 * 60 * 60;
                break;
            default:
                break;
        }
    }
    for(int i = optind; i < argc; i++) {
        list_main(argv[i]);
    }
    return 0;
}
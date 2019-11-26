# 实验 3.2 Linux 下简易 Shell 的实现

###  实验内容

在这次实验，我们实现了一个简单的带有重定向功能和管道功能的管道。

### 设计思路

整个 shell 分为两部分，前端部分负责读取用户输入并解析，后端负责使用 `fork` 等系统调用执行指令。

### 实现代码

```C
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAXARGLEN 64
#define MAXARGS 128
#define BUFFER_SIZE 1024

#define TOK_END 0
#define TOK_STR 1
#define TOK_APP 2
#define TOK_OUT 3
#define TOK_INF 4
#define TOK_PIP 5

#define TYPE_IN 1
#define TYPE_OUT 2
#define TYPE_APPEND 3
#define TYPE_INOUT 4
#define TYPE_INAPPEND 5

static char command_buffer[BUFFER_SIZE];
static char token_buffer[BUFFER_SIZE];
static int command_index;

typedef enum {
    TYPE_EXEC,
    TYPE_PIPE,
    TYPE_REDI,
    TYPE_EROR
} type_t;

typedef struct meta_command {
    type_t type;
} meta_command;

typedef struct exec_command {
    type_t type;
    int argc;
    char *argv[MAXARGS];
} exec_command;

typedef struct redi_command {
    type_t type;
    exec_command *real_command;
    char in_file[MAXARGLEN];
    char out_file[MAXARGLEN];
    int redi_type;
} redi_command;

typedef struct pipe_command {
    type_t type;
    meta_command *left;
    meta_command *right;
} pipe_command;

void get_command() {
    command_index = 0;
    printf("=> ");
    gets(command_buffer);
}

bool is_valid_char(char c) {
    if(c == '>' || c == '<' || c == '|' || c == '\t' || c == '\n' || c == ' ')
        return false;
    return true;
}

int get_basic_string() {
    int token_index = 0;
    while(command_buffer[command_index] != '\0' &&
            is_valid_char(command_buffer[command_index])) {
        token_buffer[token_index++] = command_buffer[command_index];
        command_index ++;
    }
    token_buffer[token_index++] = '\0';
    return token_index - 1 == 0 ? TOK_END : TOK_STR;
}

int lex() {
    int token_index = 0;
    token_buffer[0] = '\0';
    while(command_buffer[command_index] != '\0') {
        switch (command_buffer[command_index]) {
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                command_index += 1;
                break;
            case '>':
                if(command_buffer[command_index + 1] == '>') {
                    token_buffer[0] = '>';
                    token_buffer[1] = '>';
                    token_buffer[2] = '\0';
                    command_index += 2;
                    return TOK_APP;
                } else {
                    token_buffer[0] = '>';
                    token_buffer[1] = '\0';
                    command_index += 1;
                    return TOK_OUT;
                }
            case '<':
                token_buffer[0] = '<';
                token_buffer[1] = '\0';
                command_index += 1;
                return TOK_INF;
            case '|':
                token_buffer[0] = '|';
                token_buffer[1] = '\0';
                command_index += 1;
                return TOK_PIP;
            default:
                return get_basic_string();
        }
    }
    return TOK_END;
}

pipe_command *build_pipe_command(meta_command *left, meta_command *right) {
    pipe_command *command = malloc(sizeof(pipe_command));
    memset(command, 0, sizeof(pipe_command));
    command->type = TYPE_PIPE;
    command->left = left;
    command->right = right;
    return command;
}

redi_command *build_redi_command(exec_command *real_command, char *in_file, char *out_file, int redi_type) {
    redi_command *command = malloc(sizeof(redi_command));
    memset(command, 0, sizeof(redi_command));
    command->type = TYPE_REDI;
    command->real_command = real_command;
    strcpy(command->in_file, in_file);
    strcpy(command->out_file, out_file);
    command->redi_type = redi_type;
    return command;
}

meta_command *parse_command() {
    exec_command *command = malloc(sizeof(exec_command));
    memset(command, 0, sizeof(exec_command));
    command->type = TYPE_EXEC;
    int token_type;
    while ((token_type = lex()) == TOK_STR) {
        command->argv[command->argc] = malloc(strlen(token_buffer) + 1);
        strcpy(command->argv[command->argc], token_buffer);
        command->argc ++;
    }
    switch (token_type) {
        case TOK_PIP:
            return (meta_command *)build_pipe_command((meta_command *)command,  (meta_command *)parse_command());
        case TOK_INF:
        case TOK_OUT:
        case TOK_APP:
            lex();
            int curr_type = token_type;
            char buffered_op[MAXARGLEN];
            strcpy(buffered_op, token_buffer);
            int next_type = lex();
            if(next_type == TOK_STR) {
                command->type = TYPE_EROR;
                return (meta_command *)command;
            } else if(next_type == TOK_PIP) {
                if(curr_type == TOK_INF)
                    return (meta_command *)build_pipe_command((meta_command *) build_redi_command(
                            command, buffered_op, "", TYPE_IN), (meta_command *)parse_command());
                if(curr_type == TOK_OUT)
                    return (meta_command *)build_pipe_command((meta_command *) build_redi_command(
                            command, "", buffered_op, TYPE_OUT), (meta_command *)parse_command());
                if(curr_type == TOK_APP)
                    return (meta_command *)build_pipe_command((meta_command *) build_redi_command(
                            command, "", buffered_op, TYPE_APPEND), (meta_command *)parse_command());
            } else if(next_type == TOK_END) {
                if(curr_type == TOK_INF)
                    return (meta_command *) build_redi_command(command, buffered_op, "", TYPE_IN);
                if(curr_type == TOK_OUT)
                    return (meta_command *) build_redi_command(command, "", buffered_op, TYPE_OUT);
                if(curr_type == TOK_APP)
                    return (meta_command *) build_redi_command(command, "", buffered_op, TYPE_APPEND);
            } else if (next_type == TOK_OUT || next_type == TOK_APP) {
                if(curr_type == TOK_OUT || curr_type == TOK_APP || lex() != TOK_STR) {
                    command->type = TYPE_EROR;
                    return (meta_command *)command;
                } else {
                    return (meta_command *) build_redi_command(command, token_buffer,
                            buffered_op, next_type == TOK_OUT ? TYPE_INOUT : TYPE_INAPPEND);
                }
            } else if(next_type == TOK_INF) {
                if(curr_type == TOK_INF || lex() != TOK_STR) {
                    command->type = TYPE_EROR;
                    return (meta_command *)command;
                } else {
                    return (meta_command *) build_redi_command(command, buffered_op, token_buffer, curr_type == TOK_OUT ? TYPE_INOUT : TYPE_INAPPEND);
                }
            }
            break;
        default:
            return (meta_command *)command;
    }
}

void run_command(meta_command *command) {
    int fd;
    int pipe_fd[2];
    switch (command->type) {
    case TYPE_EXEC:
        execvp(((exec_command *)command)->argv[0], ((exec_command *)command)->argv);
        break;
    case TYPE_REDI:
        switch(((redi_command *)command)->redi_type) {
        case TYPE_IN:
            if((fd = open(((redi_command *)command)->in_file, O_RDONLY)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            break;
        case TYPE_APPEND:
            if((fd = open(((redi_command *)command)->out_file, O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            break;
        case TYPE_OUT:
            if((fd = open(((redi_command *)command)->out_file, O_WRONLY | O_CREAT, 0666)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            break;
        case TYPE_INOUT:
            if((fd = open(((redi_command *)command)->out_file, O_WRONLY | O_CREAT, 0666)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            if((fd = open(((redi_command *)command)->in_file, O_RDONLY)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            break;
        case TYPE_INAPPEND:
            if((fd = open(((redi_command *)command)->out_file, O_WRONLY | O_APPEND | O_CREAT, 0666)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            if((fd = open(((redi_command *)command)->in_file, O_RDONLY)) == -1) {
                puts("Open file failed.");
                return;
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            break;
        }
        run_command((meta_command *)((redi_command *)command)->real_command);
        break;
    case TYPE_PIPE:
        pipe(pipe_fd);
        if(fork() == 0) {
            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            run_command((meta_command *)((pipe_command *)command)->left);
        }
        if(fork() == 0) {
            dup2(pipe_fd[0], STDIN_FILENO);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            run_command((meta_command *)((pipe_command *)command)->left);
        }
        wait();
        wait();
        break;
    default:
        break;
    }
}

int main() {
    while(1) {
        get_command();
        if (!strcmp(command_buffer, "exit")) {
            exit(0);
        }
        if(fork() == 0) {
            meta_command *command = parse_command();
            run_command(command);
        }
        wait();
    }
    return 0;
}
```

### 演示效果

```bash
name1e5s@ubuntu:~$ gcc Lab-3.c
Lab-3.c: In function ‘get_command’:
Lab-3.c:66:5: warning: implicit declaration of function ‘gets’; did you mean ‘fgets’? [-Wimplicit-function-declaration]
   66 |     gets(command_buffer);
      |     ^~~~
      |     fgets
Lab-3.c: In function ‘run_command’:
Lab-3.c:295:9: warning: implicit declaration of function ‘wait’ [-Wimplicit-function-declaration]
  295 |         wait();
      |         ^~~~
/usr/bin/ld: /tmp/ccjpGWCM.o: in function `get_command':
Lab-3.c:(.text+0x30): warning: the `gets' function is dangerous and should not be used.
name1e5s@ubuntu:~$ ./a.out
=> ls -la > test.txt
=> cat test.txt
total 136
drwxr-xr-x 20 name1e5s name1e5s  4096 Oct 28 07:34 .
drwxr-xr-x  3 root     root      4096 Oct 25 06:56 ..
-rwxrwxr-x  1 name1e5s name1e5s 17840 Oct 28 07:34 a.out
-rw-------  1 name1e5s name1e5s  3068 Oct 28 07:29 .bash_history
-rw-r--r--  1 name1e5s name1e5s   220 Oct 25 06:56 .bash_logout
-rw-r--r--  1 name1e5s name1e5s  3771 Oct 25 06:56 .bashrc
drwxr-x--- 15 name1e5s name1e5s  4096 Oct 28 07:23 .cache
drwxr-x--- 15 name1e5s name1e5s  4096 Oct 27 03:35 .config
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Desktop
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Documents
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Downloads
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:23 .fontconfig
drwx------  3 name1e5s name1e5s  4096 Oct 25 07:04 .gnupg
drwxr-xr-x  3 name1e5s name1e5s  4096 Oct 27 03:40 lab2
-rwxrw-rw-  1 name1e5s name1e5s 10486 Oct 28 07:29 Lab-3.c
drwx------  3 name1e5s name1e5s  4096 Oct 25 07:05 .local
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Music
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Pictures
drwx------  3 name1e5s name1e5s  4096 Oct 25 07:42 .pki
-rw-r--r--  1 name1e5s name1e5s   807 Oct 25 06:56 .profile
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Public
drwx------  2 name1e5s name1e5s  4096 Oct 25 08:15 .ssh
-rw-r--r--  1 name1e5s name1e5s     0 Oct 25 07:12 .sudo_as_admin_successful
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Templates
-rw-rw-r--  1 name1e5s name1e5s  1744 Oct 28 07:33 test.txt
drwxr-xr-x  2 name1e5s name1e5s  4096 Oct 25 07:05 Videos
drwxr-xr-x  3 name1e5s name1e5s  4096 Oct 25 07:42 .vscode
drwxrwxr-x  5 name1e5s name1e5s  4096 Oct 25 08:17 .vscode-server
-rw-r--r--  1 name1e5s name1e5s   280 Oct 25 08:17 .wget-hsts
=> exit
name1e5s@ubuntu:~$
```


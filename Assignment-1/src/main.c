#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

char *strip(char *string) {
    if (string == NULL) {
        return NULL;
    }

    int length = strlen(string);
    char *strippedString = (char *) malloc(length*sizeof(char));

    int x = 0;
    for (int i=0; i < length; i++) {
        if (isspace(string[i]) == 0) {
            strippedString[x++] = string[i];
        }
    }
    strippedString[x] = '\0';

    return strippedString;
}

int isNumeric(char *string) {
    for (int i=0; i < strlen(string); i++) {
        if (isdigit(string[i]) == 0) {
            return 0;
        }
    }
    return 1;
}

int isBuiltin(char *command) {
    char *internal[7] = {"exit", "cd", "echo", "pwd", "type", "help", "crimge"};
    char *external[5] = {"ls", "cat", "date", "rm", "mkdir"};
    for (int i=0; i < 7; i++) {
        if (strcmp(command, internal[i]) == 0) {
            return 1;
        }
        if (i < 5 && strcmp(command, external[i]) == 0) {
            return -1;
        }
    }
    return 0;
}


int binPath(char *command) {
    if (strcmp(command, "ls") == 0) {
        return 1;
    }
    else if (strcmp(command, "cat") == 0) {
        return 2;
    }
    else if (strcmp(command, "date") == 0) {
        return 3;
    }
    else if (strcmp(command, "rm") == 0) {
        return 4;
    }
    else if (strcmp(command, "mkdir") == 0) {
        return 5;
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    int maxSize = 256;
    char *username = (char *) getlogin();

    printf("oshell-2.3.1 \n");
    printf("login as %s \n", username);

    char *cwd = (char *) malloc(maxSize*sizeof(char));
    getcwd(cwd, maxSize);
    cwd = strrchr(cwd, '/') + 1;

    char *external[6] = {
        realpath("./bin/main", NULL),
        realpath("./bin/ls", NULL),
        realpath("./bin/cat", NULL),
        realpath("./bin/date", NULL),
        realpath("./bin/rm", NULL),
        realpath("./bin/mkdir", NULL),
    };

    while (1) {
        printf("[%s@oshell %s] $ ", username, cwd);

        char *command = (char *) malloc(maxSize*sizeof(char));
        fgets(command, maxSize, stdin);

        char **args = (char **) malloc(maxSize*sizeof(char *));
        memset(args, 0, maxSize*sizeof(char *));

        int countArgs = 0;
        int n = 1;

        char *token = strip(strtok(command, " "));
        while (token != NULL) {
            if (countArgs == n*maxSize) {
                args = (char **) realloc(args, (++n)*maxSize);
            }
            args[countArgs++] = token;
            token = strip(strtok(NULL, " "));
        }

        if (strcmp(args[0], "") == 0) {
            continue;
        }

        else if (strcmp(args[0], "exit") == 0) {
            // Internal Command (EXTRA)

            if (args[2] != NULL) {
                printf("-bash: exit: too many arguments \n");
                continue;
            }

            if (args[1] != NULL && !isNumeric(args[1])) {
                printf("-bash: exit: %s: numeric argument required \n", args[1]);
            }
            printf("logout (tty1) \n");

            if (args[1] != NULL) {
                exit(atoi(args[1]));
            }
            else {
                exit(EXIT_SUCCESS);
            }
        }

        else if (strcmp(args[0], "cd") == 0) {
            // Internal Command

            if (args[1] == NULL || strcmp(args[1], "") == 0) {
                continue;
            }
            if (args[1] != NULL && args[1][0] == '-') {
                printf("-bash: cd: %s: invalid option \n", args[1]);
                continue;
            }
            if (args[2] != NULL) {
                printf("-bash: cd: too many arguments /n");
                continue;
            }

            if (chdir(args[1]) == 0) {
                cwd = strrchr(getcwd(cwd, maxSize), '/') + 1;
            }
            else {
                printf("-bash: cd: %s: No such file or directory \n", args[1]);
            }
        }

        else if (strcmp(args[0], "echo") == 0) {
            // Internal Command

            if (args[1] == NULL || strcmp(args[1], "") == 0) {
                printf("\n");
                continue;
            }

            int flag = 1;
            if (strcmp(args[1], "-n") == 0) {
                flag = 0;
            }

            for (int i=2-flag; i < countArgs; i++) {
                printf("%s", args[i]);
                if (i != countArgs-1) {
                    printf(" ");
                }
            }

            if (flag) {
                printf("\n");
            }
        }

        else if (strcmp(args[0], "pwd") == 0) {
            // Internal Command

            if (args[1] != NULL && args[1][0] == '-') {
                printf("-bash: pwd: %s: invalid option \n", args[1]);
                continue;
            }

            char *currentDirectory = (char *) malloc(maxSize*sizeof(char));
            if (getcwd(currentDirectory, maxSize) != NULL) {
                printf("%s \n", currentDirectory);
            }
            else {
                perror("pwd");
                exit(EXIT_FAILURE);
            }
        }

        else if (strcmp(args[0], "type") == 0) {
            // Internal Command

            if (args[1] == NULL || strcmp(args[1], "") == 0) {
                continue;
            }

            int type = isBuiltin(args[1]);
            if (type == 1) {
                printf("%s is a shell builtin \n", args[1]);
            }
            else if (type == -1) {
                printf("%s is %s \n", args[1], external[binPath(args[1])]);
            }
            else {
                printf("-bash: type: %s: not found \n", args[1]);
                continue;
            }

            if (args[2] != NULL) {
                printf("-bash: type: %s: not found \n", args[2]);
            }
        }

        else if (strcmp(args[0], "help") == 0) {
            // External Command
            printf("Detected command: help \n");
        }

        else if (strcmp(args[0], "crimge") == 0) {
            // Easter-Egg
            if (args[1] != NULL && args[1][0] == '-') {
                printf("-bash: crimge: %s: invalid option \n", args[1]);
                continue;
            }
            printf("OS will be the death of me. O, Shell. OS, Hell. \n");
        }

        else {
            // External Commands

            int bin = binPath(args[0]);
            if (bin == 0) {
                // Invalid Command
                printf("-bash: %s: command not found \n", args[0]);
                continue;
            }

            pid_t pid = fork();

            if (pid > 0) {
                // Code to be executed by Parent
                // Wait
                int status;
                if (waitpid(pid, &status, 0) <= 0) {
                    perror("waitpid");
                }
            }
            else if (pid == 0) {
                // Code to be executed by Child
                // Exec
                if (execv(external[bin], args) == -1) {
                    perror("execv");
                }
            }
            else {
                perror("fork");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}
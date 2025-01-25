#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

// Splits the string by space and returns the array of tokens
char **tokenize(char *line) {
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for (i = 0; i < strlen(line); i++) {

    char readChar = line[i];

    if (readChar == ' ' || readChar == '\n' || readChar == '\t') {
      token[tokenIndex] = '\0';
      if (tokenIndex != 0) {
        tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
        strcpy(tokens[tokenNo++], token);
        tokenIndex = 0;
      }
    } else {
      token[tokenIndex++] = readChar;
    }
  }

  free(token);
  tokens[tokenNo] = NULL; // null terminating the token array
  return tokens;
}

char **removeLastToken(char **tokens) {
  // find the number of tokens in tokens array
  int numTokens = 0;
  while (tokens[numTokens] != NULL) {
    numTokens++;
  }

  // allocate the memory for the new tokens array
  char **newTokens = (char **)malloc(numTokens * sizeof(char *));

  // copying the tokens
  for (int i = 0; i < numTokens - 1; i++) {
    newTokens[i] = tokens[i];
  }

  // null terminating the array
  newTokens[numTokens - 1] = NULL;
  return newTokens;
}

// custom signal handler
int fgGroup = -1;
int bgGroup = -1;
void customHandler(int sig) {
  if (fgGroup == -1)
    return;
  else
    kill(-fgGroup, SIGTERM);
}

int main(int argc, char *argv[]) {

  (void)signal(SIGINT, customHandler);

  char line[MAX_INPUT_SIZE];
  char **tokens;
  int i;

  while (1) {

    // before taking any command, check if any background process has completed
    int cpidReaped = waitpid(-1, NULL, WNOHANG);
    if (cpidReaped > 0) {
      printf("\nReaped the child with pid: %d\n", cpidReaped);
    }

    // ------------ BEGIN: TAKING INPUT ------------
    bzero(line, sizeof(line));
    printf("$ ");
    scanf("%[^\n]", line);
    getchar();
    // ---------------------------------------------

    line[strlen(line)] = '\n'; // terminate with new line
    tokens = tokenize(line);

    // check the last token, if it is "&", then switch to background mode
    i = 0;
    while (tokens[i] != NULL) {
      i++;
    }

    // ------------ BACKGROUND PROCESS ------------
    if (strcmp("&", tokens[i - 1]) == 0) {
      // make the new token array removing the "&" in the original token array
      char **newtokens = removeLastToken(tokens);
      // forking a new child process
      pid_t cpid = fork();
      if (cpid < 0) {
        printf("Some error occurred!\n");
      } else if (cpid == 0) {
        int isError = execvp(newtokens[0], newtokens);
        if (isError) {
          printf("No such command found!\n");
        }
      } else {
        // if there is some bg process group, put the new process in it, else
        // create such a group
        if (bgGroup == -1) {
          bgGroup = cpid;
        }
        setpgid(cpid, bgGroup);
      }
    }

    // ------------ FOREGROUND PROCESS ------------
    else {

      // if the command is "exit"
      if (strcmp(tokens[0], "exit") == 0) {
        // kill all background processes
        pid_t cpid = fork();
        if (cpid == 0 && bgGroup != -1) {
          char str[20];
          snprintf(str, sizeof(str), "%d", -bgGroup);
          char *argv[] = {strdup("kill"), strdup("-9"), str, NULL};
          printf("Killing background processes...");
          execvp("kill", argv);
          printf("Error in killing the foreground processes...");
        } else if (cpid > 0) {
          // kill all foreground processes
          cpid = fork();
          if (cpid == 0 && fgGroup != -1) {
            char str[20];
            snprintf(str, sizeof(str), "%d", -fgGroup);
            char *argv[] = {strdup("kill"), strdup("-9"), str, NULL};
            printf("Killing foreground processes...");
            execvp("kill", argv);
            printf("Error in killing the foreground processes...");
          } else if (cpid > 0) {
            // reap all the dead children
            int check = 1;
            while (check) {
              int res1 = waitpid(-bgGroup, NULL, WNOHANG) == -1 ? 0 : 1;
              int res2 = waitpid(-fgGroup, NULL, WNOHANG) == -1 ? 0 : 1;
              check = res1 || res2;
            }
          }
        }
        // kill this shell process
        kill(getpid(), SIGINT);
      }

      // if the command is "cd"
      else if (strcmp(tokens[0], "cd") == 0) {
        int chDir = chdir(tokens[1]);
        if (chDir == 0) {
          char *cwd = getcwd(NULL, 0);
          printf("Current directory: %s\n", cwd);
        } else {
          printf("No such directory!\n");
        }
      }

      else {
        // forking a child process to run the command
        pid_t cpid = fork();
        if (cpid < 0) {
          printf("Something wrong occurred!\n");
        } else if (cpid == 0) {
          // we have "tokens" as the argument array
          int isError = execvp(tokens[0], tokens);
          if (isError) {
            printf("No such command found!\n");
          }
        } else {
          // wait only for foreground child process to complete
          if (fgGroup == -1) {
            fgGroup = cpid;
          }
          setpgid(cpid, fgGroup);
          waitpid(cpid, NULL, 0);
        }
      }
    }

    // Freeing the allocated memory
    for (i = 0; tokens[i] != NULL; i++) {
      free(tokens[i]);
    }
    free(tokens);
  }

  return 0;
}
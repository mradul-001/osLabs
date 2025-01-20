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

int main(int argc, char *argv[]) {

  char line[MAX_INPUT_SIZE];
  char **tokens;
  int i;
  int bgGroup = 0;

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
        // parent process
        // do nothing, don't wait
        if (bgGroup != 0) {
          setpgid(cpid, bgGroup);
        } else {
          setpgid(cpid, cpid);
          bgGroup = getpgid(cpid);
        }
      }
    }

    // ------------ FOREGROUND PROCESS ------------
    else {

      // if the command is "exit"
      if (strcmp(tokens[0], "exit") == 0) {
        char str[20]; // 20 digit long pgid
        // converting bgGroup to string
        snprintf(str, sizeof(str), "%d", -bgGroup);
        // // printf("%s", str);
        // char *args[] = {"kill", "-9", str, NULL};
        // int errorId = execvp("kill", args);
        // if (errorId == -1) {
        //   printf("Some error killing the shell.");
        // }
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

      } else {
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
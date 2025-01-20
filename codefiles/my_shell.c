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



int main(int argc, char *argv[]) {

  char line[MAX_INPUT_SIZE];
  char **tokens;
  int i;

  while (1) {

    /* BEGIN: TAKING INPUT */
    bzero(line, sizeof(line));
    printf("$ ");
    scanf("%[^\n]", line);
    getchar();
    /* END: TAKING INPUT */

    line[strlen(line)] = '\n'; // terminate with new line
    tokens = tokenize(line);

    // check the last token, if it is "&", then switch to background mode
    i = 0;
    while (tokens[i] != NULL) {
      i++;
    }

    if (strcmp("&", tokens[i - 1]) == 0) {
      // make the new token array removing the "&" in the original token array
      char **newtokens = newtoken(tokens);
      // test
      int k = 0;
      while (newtokens[k] != NULL) {
        printf("%s\n", newtokens[k]);
      }
    } else {
      // if the command is "cd"
      if (strcmp(tokens[0], "cd") == 0) {
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
          wait(NULL);
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

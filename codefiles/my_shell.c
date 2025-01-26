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

// ------------------------------------
int bgGroup = 0;
int bg_count = 0;
int fgGroup = 0;
void handler(int sig) { return; }
// ------------------------------------

int main(int argc, char *argv[]) {

  (void)signal(SIGINT, handler);

  char line[MAX_INPUT_SIZE];
  char **tokens;
  int i;

  while (1) {

    // before taking any command, check if any background process has completed
    int cpidReaped = waitpid(-1, NULL, WNOHANG);
    if (cpidReaped > 0) {
      printf("\nReaped the child with pid: %d\n", cpidReaped);
      bg_count--;
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
        bg_count++;
        // set bggroup id
        if (bgGroup == 0) {
          bgGroup = cpid;
        }
        setpgid(cpid, bgGroup);
      }
    }

    // ------------ FOREGROUND PROCESS ------------

    else {

      if (strcmp("exit", tokens[0]) == 0) {

        // kill background processes
        if (bgGroup != 0)
          kill(-bgGroup, SIGTERM);

        while (bg_count > 0) {
          waitpid(-bgGroup, NULL, WNOHANG);
          bg_count--;
        }

        // kill the shell process
        kill(-getpid(), SIGTERM);
      }

      // if the command is "cd"
      if (strcmp(tokens[0], "cd") == 0) {
        int chDir = chdir(tokens[1]);
        if (chDir == 0) {
          char *cwd = getcwd(NULL, 0);
          printf("Current directory: %s\n", cwd);
        } else {
          printf("No such directory!\n");
        }
      }

      int num_subproc = 1;
      int prev = 0;

      for (int i = 0; i < MAX_NUM_TOKENS; i++) {
        if (tokens[i] == NULL)
          break;
        if (!strcmp(tokens[i], "&&")) {
          tokens[i] = NULL;
          pid_t cpid = fork();
          if (cpid == 0) {
            if (fgGroup == 0)
              fgGroup = cpid;
            setpgid(cpid, fgGroup);
            execvp(*(tokens + prev), (tokens + prev));
            printf("Shell: Incorrect Command : %s\n", *(tokens + prev));
            exit(0);
          } else {
            wait(NULL);
            prev = i + 1;
          }
          continue;
        }
        if (!strcmp(tokens[i], "&&&")) {
          tokens[i] = NULL;
          num_subproc++;
          pid_t cpid = fork();
          if (cpid == 0) {
            if (fgGroup == 0)
              fgGroup = cpid;
            setpgid(cpid, fgGroup);
            execvp(*(tokens + prev),
                   (tokens + prev)); // Execute the current command
            printf("Shell: Incorrect Command : %s\n", *(tokens + prev));
            exit(0);
          } else {
            prev = i + 1;
          }
          continue;
        }
      }
      // Remember, we still have the final command left, so let's execute that
      // too
      if (fork() == 0) {
        execvp(*(tokens + prev), tokens + prev);
        printf("Shell: Incorrect Command : %s\n", tokens[0]);
        exit(0);
      } else {

        for (int i = 0; i < num_subproc; i++) {
          wait(NULL);
        }
        // Must have as many wait statements as the processes we created
        // For && we were reaping instantly, for &&& we incremented the
        // counter
      }
    }

    // ------------ Freeing the allocated memory ------------
    for (i = 0; tokens[i] != NULL; i++) {
      free(tokens[i]);
    }
    free(tokens);
  }
  // ------------------------------------------------------
  return 0;
}
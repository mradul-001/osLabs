#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the string by space and returns the array of tokens
 *
 */
char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
    int i, tokenIndex = 0, tokenNo = 0;

    for (i = 0; i < strlen(line); i++)
    {

        char readChar = line[i];

        if (readChar == ' ' || readChar == '\n' || readChar == '\t')
        {
            token[tokenIndex] = '\0';
            if (tokenIndex != 0)
            {
                tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
                strcpy(tokens[tokenNo++], token);
                tokenIndex = 0;
            }
        }
        else
        {
            token[tokenIndex++] = readChar;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;
    return tokens;
}

int fgProcess = 0;

void alarmHandler(int sig)
{
    // kill the foreground process
    if (fgProcess == 0)
    {
        return;
    }
    else
    {
        kill(fgProcess, SIGTERM);
    }
    printf("KILLED CHILD AFTER 10 SECONDS\n");
}

int main(int argc, char *argv[])
{
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i;

    (void)signal(SIGALRM, alarmHandler);

    while (1)
    {

        char buffer[256];
        getcwd(buffer, sizeof(buffer));

        /* BEGIN: TAKING INPUT */
        bzero(line, sizeof(line));
        printf("%s $ ", buffer);
        scanf("%[^\n]", line);
        getchar();

        // printf("Command entered: %s (remove this debug output later)\n", line);
        /* END: TAKING INPUT */

        line[strlen(line)] = '\n'; // terminate with new line
        tokens = tokenize(line);

        // do whatever you want with the commands, here we just print them

        // for(i=0;tokens[i]!=NULL;i++){
        // 	printf("found token %s (remove this debug output later)\n", tokens[i]);
        // }

        if (tokens[0] == NULL)
            continue;

        if (strcmp(tokens[0], "cd") == 0)
        {
            int status = chdir(tokens[1]);
            if (status == -1)
            {
                printf("No such file or directory.\n");
            }
        }
        else
        {
            pid_t cpid = fork();
            if (cpid < 0)
            {
                printf("Fork failed.\n");
            }
            else if (cpid == 0)
            {
                execvp(tokens[0], tokens);
                exit(1);
            }
            else
            {
                fgProcess = cpid;
                alarm(10);
                int status;
                pid_t w = waitpid(cpid, &status, 0);
                if (w > 0)
                    fgProcess = 0;
                if (WIFEXITED(status))
                {
                    printf("EXITSTATUS: %d\n", (WEXITSTATUS(status)));
                }
            }
        }

        // Freeing the allocated memory
        for (i = 0; tokens[i] != NULL; i++)
        {
            free(tokens[i]);
        }
        free(tokens);
    }
    return 0;
}

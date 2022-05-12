#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

int getArguments(char *buffer, char *myargv[]);
void addPath(char *pathVar, char *myPaths[]);

int main(int argc, char *argv[])
{
    while (1)
    {
        printf("wish > ");
        size_t bufsize = 32;
        char *buffer = (char *)malloc(bufsize * sizeof(char));
        getline(&buffer, &bufsize, stdin);
        char *myargv[20];
        int numArgs = getArguments(buffer, myargv);
        char error_message[30] = "An error has occurred\n";

        char *myPaths[20];
        char destbin[10] = "/bin/";
        char destusr[20] = "/usr/bin";

        addPath(destbin, myPaths);
        addPath(destusr, myPaths);

        if (!strcmp(myargv[0], "exit"))
        {
            exit(0);
        }
        int rc = fork();
        if (rc < 0)
        {
            printf("fork failed\n");
            exit(1);
        }
        else if (rc == 0)
        {
            FILE *fp = NULL;
            if (numArgs > 2 && !strcmp(myargv[numArgs - 2], ">"))
            {
                fp = freopen(myargv[numArgs - 1], "w", stdout);
                myargv[numArgs - 2] = NULL;
                if (!fp)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
            }
            if (!strcmp(myargv[0], "cd"))
            {
                if (myargv[1] == NULL || myargv[2] != NULL)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                }
                else
                {
                    chdir(myargv[1]);
                }
            }
            else if (!strcmp(myargv[0], "path"))
            {
                for (int i = 1; myargv[i] != NULL; i++)
                {
                    addPath(myargv[i], myPaths);
                }
            }
            else
            {
                if (!access(myargv[0], X_OK))
                {
                    execv(myargv[0], myargv);
                }
                else
                {
                    strcat(destbin, myargv[0]);
                    strcat(destusr, myargv[0]);
                    if (!access(destbin, X_OK))
                    {
                        myargv[0] = destbin;
                        execv(myargv[0], myargv);
                    }
                    else if (!access(destusr, X_OK))
                    {
                        myargv[0] = destusr;
                        execv(myargv[0], myargv);
                    }
                }
            }
            if (fp)
            {
                fclose(fp);
            }
        }
        else
        {
            wait(NULL);
        }
    }

    return 0;
}

void addPath(char *pathVar, char *myPaths[])
{
    static int i = 0;
    myPaths[i] = pathVar;
    i++;
}

int getArguments(char *buffer, char *myargv[])
{
    buffer[strcspn(buffer, "\n")] = 0;
    int currentStart = 0;
    int numArg = 0;
    int i = 0;
    int numChar = 0;
    bool check = true;
    for (i = 0; buffer[i] != 0; i++)
    {
        if ((buffer[i] != 32 && buffer[i] != 9) && check)
        {
            numChar = 1;
            currentStart = i;
            check = false;
        }
        else if ((buffer[i] == 32 || buffer[i] == 0 || buffer[i] == 9) && !check)
        {
            void *malloc(size_t size);
            char *argv = malloc((i - currentStart) * sizeof(char));
            memcpy(argv, &buffer[currentStart], numChar);
            myargv[numArg] = argv;
            numChar = 0;
            numArg++;
            check = true;
        }
        else if (!check)
        {
            numChar++;
        }
    }
    void *malloc(size_t size);
    char *argv = malloc((i - currentStart) * sizeof(char));
    memcpy(argv, &buffer[currentStart], i);
    myargv[numArg] = argv;
    myargv[numArg + 1] = NULL;
    return numArg + 1;
}

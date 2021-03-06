#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

int getArguments(char *buffer, char *myargv[]);
void addPath(char *pathVar, char *myPaths[]);
void runInteractiveShell(char *myPaths[], char error_message[], char *myargv[], int numPaths, int numArgs);
void clearPath(char *myPaths[]);
int numPaths = 0;

int main(int argc, char *argv[])
{

    char *myPaths[20];
    char destbin[10] = "/bin/";
    addPath(destbin, myPaths);
    char error_message[30] = "An error has occurred\n";
    if (argc == 1)
    {
        while (1)
        {
            printf("wish > ");
            size_t bufsize = 32;
            char *buffer = (char *)malloc(bufsize * sizeof(char));
            getline(&buffer, &bufsize, stdin);
            char *myargv[20];
            int numArgs = getArguments(buffer, myargv);
            runInteractiveShell(myPaths, error_message, myargv, numPaths, numArgs);
        }
    }
    else if (argc == 2)
    {
        FILE *fp;
        char *line = NULL;
        size_t len = 0;
        ssize_t read;

        fp = fopen(argv[1], "r");
        if (fp == NULL)
            exit(EXIT_FAILURE);

        while ((read = getline(&line, &len, fp)) != -1)
        {
            char *myargv[20];
            int numArgs = getArguments(line, myargv);
            runInteractiveShell(myPaths, error_message, myargv, numPaths, numArgs);
        }

        fclose(fp);
    }

    return 0;
}

void runInteractiveShell(char *myPaths[], char error_message[], char *myargv[], int numPaths, int numArgs)
{
    int currentProStart = 0;

    if (!strcmp(myargv[0], "exit"))
    {
        if (numArgs > 1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(0);
        }
        exit(0);
    }

    for (int i = 0; myargv[i] != NULL; i++)
    {
        if (!strcmp(myargv[i], "&") || myargv[i + 1] == NULL)
        {
            int start = 0;
            int index = i;
            if (myargv[i + 1] == NULL)
            {
                index = i + 1;
            }
            for (int j = currentProStart; j < index; ++j)
            {
                myargv[start] = myargv[j];
                start++;
            }
            myargv[start] = NULL;
            numArgs = i - currentProStart;
            if (myargv[i + 1] == NULL)
            {
                numArgs++;
            }

            FILE *fp[20];
            int numFiles = 0;

            for (int i = 0; i < numArgs; i++)
            {

                if (!strcmp(myargv[i], ">"))
                {
                    if (!strcmp(myargv[numArgs - 1], ">"))
                    {
                        write(STDERR_FILENO, error_message, strlen(error_message));
                        exit(0);
                    }
                    else
                    {
                        myargv[i] = NULL;
                        for (int iter = i + 1; iter < numArgs; iter++)
                        {

                            fp[numFiles] = fopen(myargv[i], "w");
                            if (!fp[i])
                            {
                                write(STDERR_FILENO, error_message, strlen(error_message));
                            }
                            numFiles++;
                        }
                    }
                }
            }

            if (!strcmp(myargv[0], "cd"))
            {
                if (myargv[1] == NULL || myargv[2] != NULL)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
                else
                {
                    chdir(myargv[1]);
                }
            }
            else if (!strcmp(myargv[0], "path"))
            {
                clearPath(myPaths);
                for (int i = 1; myargv[i] != NULL; i++)
                {
                    char *path = strcat(myargv[1], "/");
                    addPath(path, myPaths);
                }
            }
            else
            {
                int rc = fork();
                if (rc < 0)
                {
                    printf("fork failed\n");
                    exit(1);
                }
                else if (rc == 0)
                {

                    for (int i = 0; i < numPaths; i++)
                    {
                        char *dest = strdup(myPaths[i]);
                        strcat(dest, myargv[0]);
                        if (!access(dest, X_OK))
                        {
                            myargv[0] = dest;
                            execv(myargv[0], myargv);
                        }
                    }

                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(0);
                }
                else
                {
                    wait(NULL);
                }
                for (int i = 0; i < numFiles; i++)
                {
                    fclose(fp[i]);
                }
            }

            currentProStart = i + 1;
        }
    }
}

void addPath(char *pathVar, char *myPaths[])
{
    myPaths[numPaths] = pathVar;
    numPaths++;
}
void clearPath(char *myPaths[])
{
    for (int i = 0; i < numPaths; i++)
    {
        myPaths[i] = 0;
    }
    numPaths = 0;
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

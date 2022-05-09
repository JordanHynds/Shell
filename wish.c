#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    while (1)
    {
        printf("wish > ");
        size_t bufsize = 32;
        char *buffer = (char *)malloc(bufsize * sizeof(char));
        getline(&buffer, &bufsize, stdin);

        int rc = fork();
        if (rc < 0)
        {
            printf("fork failed\n");
            exit(1);
        }
        else if (rc == 0)
        {
            buffer[strcspn(buffer, "\n")] = 0;
            char *myargv[10];
            int currentStart = 0;
            int numArg = 0;
            int i = 0;
            bool check = true;
            for (i = 0; buffer[i] != 0; i++)
            {
                if (buffer[i] != 32 && check)
                {
                    currentStart = i;
                    check = false;
                }
                if ((buffer[i] == 32 || buffer[i] == 0) && !check)
                {
                    void *malloc(size_t size);
                    char *argv = malloc((i - currentStart) * sizeof(char));
                    memcpy(argv, &buffer[currentStart], i);
                    myargv[numArg] = argv;
                    numArg++;
                    currentStart = i;
                    check = true;
                }
            }
            void *malloc(size_t size);
            char *argv = malloc((i - currentStart) * sizeof(char));
            memcpy(argv, &buffer[currentStart], i);
            myargv[numArg] = argv;
            myargv[numArg + 1] = NULL;
            char destbin[10] = "/bin/";
            char destusr[20] = "/usr/bin";
            printf("%s", myargv[0]);
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
        else
        {
            wait(NULL);
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    while (1)
    {
        printf("wish>");
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
            printf("%s", buffer);
        }
        else
        {
        }
    }
    return 0;
}

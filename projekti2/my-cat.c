#include <stdio.h>
#include <stdlib.h>

#define MAX 1024

void readFile(char *fileName)
{
    FILE *file = NULL;
    char line[MAX];

    if ((file = fopen(fileName, "r")) != NULL)
    {
        while (fgets(line, MAX, file))
        {
            fprintf(stdout, "%s", line);
        }
        fclose(file);
    }
    else
    {
        perror("my-cat: cannot open file");
        exit(1);
    }
    return;
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        readFile(argv[i]);
    }
    return 0;
}
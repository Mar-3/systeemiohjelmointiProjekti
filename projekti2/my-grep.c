#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX 1024

void readFile(char *fileName, char *keyword)
{
    FILE *file = NULL;
    char line[MAX];

    if ((file = fopen(fileName, "r")) != NULL)
    {
        while (fgets(line, MAX, file))
        {
            if (strstr(line, keyword))
            {
                fprintf(stdout, "%s", line);
            }
        }
        fclose(file);
    }
    else
    {
        perror("my-grep: cannot open file");
        exit(1);
    }
    return;
}

void readStdin(char *keyword)
{
    char line[MAX];
    char printedLines[MAX];
    strcpy(printedLines, "\n");
    printf("Please type the lines you want to use grep command on. (Empty line to stop and print.):\n");
    strcpy(line, "placeholder");
    while (!(line[1] == '\0'))
    {
        fgets(line, MAX, stdin);
        if (strstr(line, keyword))
        {
            strcat(printedLines, line);
        }
    }
    printf("Lines that contain the keyword:\n%s", printedLines);
    return;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        printf("my-grep: searchterm [file ...]\n");
        return 1;
    }
    else if (argc == 2)
    {
        readStdin(argv[1]);
    }
    else
    {
        for (int i = 2; i < argc; i++)
        {
            readFile(argv[i], argv[1]);
        }
    }
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 30

void readFile(char *fileName)
{
    FILE *file = NULL;
    unsigned char line[MAX];
    int amount;
    char character;

    if ((file = fopen(fileName, "rb")) != NULL)
    {
        // Used: https://www.codegrepper.com/code-examples/c/c+read+binary+file
        fseek(file, 0, SEEK_END);
        long fileLength = ftell(file);
        fseek(file, 0, SEEK_SET);

        for (int i = 0; i < fileLength - 5; i = i + 5)
        {
            fread(line, sizeof(char), 5, file);
            amount = line[0];
            character = (char)line[4];
            for (int i = 0; i < amount; i++)
            {
                printf("%c", character);
            }
        }
        printf("\n");
        fclose(file);
    }
    else
    {
        perror("my-zip: cannot open file");
        exit(1);
    }
    return;
}
int main(int argc, char **argv)
{

    if (argc == 1)
    {
        printf("my-zip: file1 [file2...]\n");
        return 1;
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            readFile(argv[i]);
        }
    }
    return 0;
}
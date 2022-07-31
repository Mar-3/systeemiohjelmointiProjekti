// my-zip.c - Käyttöjärjestelmä ja systeemiohjelmointi projekti 2
// Marko Jutila 31.7.2022
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUFFER 1024

void readFile(char *fileName)
{
    FILE *file = NULL;
    char line[MAXBUFFER];
    int amount = 1;
    char character;

    if ((file = fopen(fileName, "r")) != NULL)
    {
        while (fgets(line, MAXBUFFER, file) != NULL)
        {
            int i = 0;
            while (line[i] != '\0')
            {
                character = line[i];
                if (character == line[i + 1])
                {
                    amount++;
                }
                else
                {
                    fwrite(&amount, sizeof(int), 1, stdout);
                    fwrite(&character, sizeof(char), 1, stdout);
                    amount = 1;
                }
                i++;
            }
        }
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
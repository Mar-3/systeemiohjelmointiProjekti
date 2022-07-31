// reverse.c - Käyttöjärjestelmä ja systeemiohjelmointi projekti 1
// Marko Jutila 20.7.2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 1024

struct LINE
{
    char text[MAX];
    struct LINE *pNext;
};

struct LINE *readFile(char inputFile[MAX])
{
    FILE *file = NULL;
    struct LINE *pNewest = NULL;
    struct LINE *ptr = NULL;
    char cLine[MAX];

    if ((file = fopen(inputFile, "r")) != NULL)
    {
        while (fgets(cLine, MAX, file) != NULL)
        {
            if ((ptr = (struct LINE *)malloc(sizeof(struct LINE))) != NULL)
            {
                strcpy(ptr->text, cLine);
                ptr->pNext = pNewest;
                pNewest = ptr;
            }
            else
            {
                fprintf(stderr, "malloc failed.");
                exit(1);
            }
        }
    }
    else
    {
        fprintf(stderr, "Problem opening the file.");
        exit(1);
    }
    fclose(file);
    return pNewest;
}

void writeFile(char *outputFile, struct LINE *pNewest)
{
    FILE *file = NULL;
    if ((file = fopen(outputFile, "w")) != NULL)
    {
        while (pNewest != NULL)
        {
            fprintf(file, "%s", pNewest->text);
            pNewest = pNewest->pNext;
        }
    }
    else
    {
        fprintf(stderr, "Problem opening the file.");
        exit(1);
    }
    fclose(file);
    return;
}

struct LINE *userInput()
{
    struct LINE *pNewest = NULL;
    struct LINE *ptr = NULL;
    char line[MAX];
    fprintf(stdout, "Type the lines you want to be printed in reverse order:('exit' to stop)\n");

    while (strcmp(line, "exit\n"))
    {
        fgets(line, MAX, stdin);
        if (line != NULL)
        {
            if ((ptr = (struct LINE *)malloc(sizeof(struct LINE))) != NULL)
            {
                strcpy(ptr->text, line);
                ptr->pNext = pNewest;
                pNewest = ptr;
            }
            else
            {
                fprintf(stderr, "malloc failed.");
                exit(1);
            }
        }
    }
    return pNewest;
}

void freeMemory(struct LINE *pNewest)
{
    struct LINE *ptr = NULL;
    while (pNewest != NULL)
    {
        ptr = pNewest->pNext;
        free(pNewest);
        pNewest = ptr;
    }
    return;
}

void printLines(struct LINE *pNewest)
{

    while (pNewest != NULL)
    {
        printf("%s", pNewest->text);
        pNewest = pNewest->pNext;
    }
    return;
}

int main(int argc, char **argv)
{
    if (argc > 3)
    {
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }
    struct LINE *pNewest = NULL;

    char inputFile[MAX];
    char outputFile[MAX];
    if (argc == 1)
    {
        // user input reverse
        pNewest = userInput();
        printf("\nUser inputted lines in reverse order:\n");
        printLines(pNewest);
    }
    else if (argc == 2)
    {
        strcpy(inputFile, argv[1]);
        pNewest = readFile(inputFile);
        printLines(pNewest);
    }
    if (argc == 3)
    {
        strcpy(inputFile, argv[1]);
        strcpy(outputFile, argv[2]);
        if (!strcmp(inputFile, outputFile))
        {
            fprintf(stderr, "Input and output file must differ");
            exit(1);
        }
        pNewest = readFile(inputFile);
        writeFile(outputFile, pNewest);
    }
    freeMemory(pNewest);
    fprintf(stdout, "Quitting program..\n");
    exit(0);
}
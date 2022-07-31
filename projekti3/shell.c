#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX 60

int runProgram(char *programAndArgs[MAX])
{
    int rc = fork();
    if (rc < 0)
    {
        fprintf(stderr, "Fork failed\n");
        return (1);
    }
    else if (rc == 0)
    {
        execvp(programAndArgs[0], programAndArgs);
    }
    else
    {
        return 0;
    }
    return rc;
}

char *checkAccess(char *paths[MAX], char *programName)
{ // Subprogram for checking if program accessible
    int counter = 0;
    char *programPath;
    char currentDirectory[MAX];
    while (paths[counter] != NULL)
    { // Builds the possible paths for a program and checks access
        programPath = strdup(paths[0]);
        strcat(programPath, "/");
        strcat(programPath, programName);

        if (access(programPath, X_OK) == 0)
        {
            return programPath;
        }
        else
        {
            counter++;
        }
    } // checking for access in current directory
    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL)
    {
        programPath = strdup(currentDirectory);
        strcat(programPath, "/");
        strcat(programPath, programName);

        if (access(programPath, X_OK) == 0)
        {
            programPath = strdup("./");
            strcat(programPath, programName);
            return programPath;
        }
    }
    else
    {
        perror("cant get current folder.");
    }
    return NULL; // return NULL if not accessible
}

void tokenCommands(char *inputString, char *outputArray[MAX], char separator) // This subprogram is for separating the commands and argumets
{
    char *token;
    int counter = 0;
    token = strtok(inputString, &separator);
    while (token != NULL)
    {                                        // Iterating through inputstring and adding all the seperated string to outputarray
        if (strcmp(token, strdup(" ")) != 0) // checking for stray spaces and not adding them to array
        {
            outputArray[counter] = strdup(token);
            outputArray[counter + 1] = NULL;
            counter++;
        }
        token = strtok(NULL, &separator);
    }
    outputArray[counter - 1] = strdup(strtok(outputArray[counter - 1], "\n"));
    return;
}

int runOneLine(char *paths[MAX], char *line)
{
    int counter;
    char *programAndArgs[MAX];
    char *oneProgram[MAX];
    char *programPath;
    counter = 0;
    if (strcmp(line, "exit\n") == 0) // Exits the loop and program if input is exit
    {
        return 1; // return used to stop the program
    }
    if (strcmp(line, "\n") != 0) // checks for empty newline input
    {                            // If input isnt empty, separates the programs and arguments by "&"
        tokenCommands(line, programAndArgs, *"&");
    }
    counter = 0;
    while (programAndArgs[counter] != NULL)
    {
        tokenCommands(programAndArgs[counter], oneProgram, *" ");
        if (!strcmp(oneProgram[0], "cd"))
        {
            if ((chdir(oneProgram[1]) != 0) && (oneProgram[2] == NULL))
            {
                printf("Error chdir");
                exit(1);
            }
        }
        else if (!strcmp(oneProgram[0], "path"))
        {
            counter = 1;
            while (oneProgram[counter] != NULL)
            {
                paths[counter - 1] = strdup(oneProgram[counter]);
                counter++;
            }
            paths[counter] = NULL;
        }
        else
        {
            if ((programPath = checkAccess(paths, oneProgram[0])) == NULL)
            {
                perror("Couldnt get access to program.");
            }
            else
            {
                if (runProgram(oneProgram) != 0)
                {
                    perror("Problem running the program.");
                    exit(1);
                }
            }
        }
        counter++;
    }
    while (wait(NULL) > 0) // waits for all the child processes to finish before
        ;                  // Giving control back to user.
    return 0;
}

int interactiveShell()
{
    // Help for using getline(): https://c-for-dummies.com/blog/?p=1112
    char *buffer;
    char *paths[MAX];
    size_t bufferSize = 32;
    buffer = (char *)malloc(bufferSize * sizeof(char));

    paths[0] = strdup("/bin"); // Setting the default paths for programs
    paths[1] = strdup("/usr/bin");
    paths[2] = NULL;
    while (1) //
    {
        printf("wish>");
        getline(&buffer, &bufferSize, stdin);
        runOneLine(paths, buffer);
    }
    return 0;
}

int batchShell(char *fileName)
{
    char *paths[MAX];
    paths[0] = strdup("/bin"); // Setting the default paths for programs
    paths[1] = strdup("/usr/bin");
    paths[2] = NULL;
    size_t bufferSize = 32;
    FILE *file = NULL;
    char *line = NULL;

    line = (char *)malloc(sizeof(char) * bufferSize);

    if ((file = fopen(fileName, "r")) == NULL)
    {
        perror("Couldnt open file");
        exit(1);
    }
    while (fgets(line, MAX, file) != NULL)
    {
        if (runOneLine(paths, line) == 1)
        {
            perror("Error");
            exit(1);
        }
    }
    fclose(file);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        interactiveShell();
    }
    else
    {
        for (int i = 1; i < argc; i++)
        {
            batchShell(argv[i]);
        }
    }
    printf("Wish exiting successfully.\n");

    return 0;
}
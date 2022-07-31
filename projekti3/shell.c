#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX 60

void freeMemory(char *array[MAX])
{
    for (int i = 0; array[i] != NULL; i++)
    {
        free(array[i]);
        array[i] = NULL;
    }
}

int runProgram(char *programAndArgs[MAX], char *programPath) // This subprogram forks a new process and runs the
{                                                            // wanted command with arguments in a new process.
    char error_message[30] = "An error has occurred\n";
    int rc = fork();
    if (rc < 0)
    {
        write(STDERR_FILENO, error_message, strlen(error_message));
        return (1);
    }
    else if (rc == 0)
    {
        execvp(programPath, programAndArgs);
    }
    else
    {
        free(programPath);
        return 0;
    }
    free(programPath);
    return rc;
}

char *checkAccess(char *paths[MAX], char *programName)
{ // Subprogram for checking if program accessible
    char *programPath = (char *)malloc(sizeof(char) * MAX);
    char currentDirectory[MAX];
    for (int i = 0; paths[i] != NULL; i++)
    { // Builds the possible paths for a program and checks access
        strcpy(programPath, paths[i]);
        strcat(programPath, "/");
        strcat(programPath, programName);
        if (access(programPath, X_OK) == 0)
        {
            return programPath; // Return path to program if accessible
        }
    } // checking for access in current directory
    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL)
    {
        strcpy(programPath, currentDirectory);
        strcat(programPath, "/");
        strcat(programPath, programName);

        if (access(programPath, X_OK) == 0)
        {
            programPath = strdup("./");
            strcat(programPath, programName);
            return programPath;
        }
    }
    free(programPath);
    return NULL; // return NULL if not accessible
}

void tokenCommands(char *inputString, char *outputArray[MAX], char separator) // This subprogram is for separating the commands and argumets
{
    char *token = NULL;
    int counter = 0;
    token = strtok(inputString, &separator);
    while (token != NULL)
    {                                // Goes through inputstring and adds all the seperated strings to an output array
        if (strcmp(token, " ") != 0) // checking for stray spaces and not adding them to array
        {
            outputArray[counter] = strdup(token);
            counter++;
        }
        token = strtok(NULL, &separator);
    }
    token = strtok(outputArray[counter - 1], "\n"); // Ignoring newline \n.
    outputArray[counter - 1] = token;
    outputArray[counter] = NULL;
    return;
}

int runOneLine(char *paths[MAX], char *line)
{
    char error_message[30] = "An error has occurred\n";
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
        if (strcmp(oneProgram[0], "cd") == 0) // if first command is cd change current directory
        {
            if ((chdir(oneProgram[1]) != 0) && (oneProgram[2] == NULL))
            {
                write(STDERR_FILENO, error_message, strlen(error_message));
                break;
            }
        }
        else if (strcmp(oneProgram[0], "path") == 0) // if first command is path, change the paths to
        {                                            // search programs from.
            freeMemory(paths);
            for (int i = 1; oneProgram[i] != NULL; i++)
            {
                paths[i - 1] = strdup(oneProgram[i]);
                printf("%s \n", paths[i - 1]);
                paths[i] = NULL;
            }
        }
        else
        {
            if ((programPath = checkAccess(paths, oneProgram[0])) == NULL)
            {
                printf("Couldnt get access to program.\n");
            }
            else
            {
                for (int i = 0; oneProgram[i] != NULL; i++) // Used: https://stackoverflow.com/questions/584868/rerouting-stdin-and-stdout-from-c
                {                                           // checking for file output sign ">" and redirecting stdout to output file if possible
                    if ((strcmp(oneProgram[i], ">") == 0))
                    {
                        if ((freopen(oneProgram[i + 1], "w", stdout)) == NULL)
                        {
                            write(STDERR_FILENO, error_message, strlen(error_message));
                            exit(1);
                        }
                        free(oneProgram[i + 1]);
                        free(oneProgram[i]);
                        oneProgram[i] = NULL;
                    }
                }
                strcpy(oneProgram[0], programPath);
                if (runProgram(oneProgram, strdup(programPath)) != 0)
                {
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    exit(1);
                }
            }
            free(programPath);
        }
        counter++;
        if (oneProgram[0] != NULL)
        {
            freeMemory(oneProgram);
        }
    }
    while (wait(NULL) > 0) // waits for all the child processes to finish before
        ;                  // Giving control back to user.

    // Setting the stdout back in case it was redirected to a file.
    freopen("/dev/tty", "w", stdout); // https://stackoverflow.com/questions/29154056/redirect-stdout-to-a-file
    freeMemory(programAndArgs);
    return 0;
}

void interactiveShell()
{
    // Help for using getline(): https://c-for-dummies.com/blog/?p=1112
    char *buffer;
    char *paths[MAX];
    size_t bufferSize = 32;
    buffer = (char *)malloc(bufferSize * sizeof(char));
    int stop = 0;

    paths[0] = strdup("/bin"); // Setting the default paths for programs
    paths[1] = strdup("/usr/bin");
    paths[2] = NULL;
    while (stop == 0) // Interactive shells main loop for user input
    {
        printf("wish>");
        getline(&buffer, &bufferSize, stdin);
        stop = runOneLine(paths, buffer); // RunOneLine returns 1 when exit is typed, ending the loop.
    }
    freeMemory(paths);
    free(buffer);
    return;
}

void batchShell(char *fileName)
{
    char error_message[30] = "An error has occurred\n";
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
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }
    while (fgets(line, MAX, file) != NULL)
    {
        if (runOneLine(paths, line) == 1)
        {
            write(STDERR_FILENO, error_message, strlen(error_message));
            exit(1);
        }
    }
    fclose(file);
    freeMemory(paths);
    free(line);
    return;
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
    printf("Shell exiting successfully.\n");
    exit(0);
}
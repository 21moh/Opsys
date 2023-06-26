#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>


bool checkQueenPlacement(char** grid, int row, int column, int m, int n) {
    //create copy grid with possible queen placement
    char** copyGrid = (char**)calloc(m, sizeof(char*));
    for (int i = 0; i < m; i++) {
        *(copyGrid+i) = (char*)calloc(n, sizeof(char));
    }
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            *(*(copyGrid + i) + j) = *(*(grid + i) + j);
        }
    }
    // check for attackers vertically |
    for (int i = 0; i < m; i++) {
        if (*(*(copyGrid + i) + column) == 'Q') {
            for (int i = 0; i < m; i++) {
                free(*(copyGrid + i));
            }
            free(copyGrid);
            return false;
        }
    }
    // check for attackers horizontally ---
    for (int j = 0; j < n; j++) {
        if (*(*(copyGrid + row) + j) == 'Q') {
            for (int i = 0; i < m; i++) {
                free(*(copyGrid + i));
            }
            free(copyGrid);
            return false;
        }
    }
    // check for attackers diagonally / \   //
    // check mid to bottem right
    int rowCheck = row;
    int colCheck = column;
    while(rowCheck != m && colCheck != n) {
        rowCheck++;
        colCheck++;
        if (rowCheck >= m) {
            break;
        }
        if (colCheck >= n) {
            break;
        }
        if (*(*(copyGrid + rowCheck) + colCheck) == 'Q') {
            for (int i = 0; i < m; i++) {
                free(*(copyGrid + i));
            }
            free(copyGrid);
            return false;
        }
    }
    // check mid to top left
    rowCheck = row;
    colCheck = column;
    while(rowCheck != 0 && colCheck != 0) {
        rowCheck--;
        colCheck--;
        if (rowCheck < 0) {
            break;
        }
        if (colCheck < 0) {
            break;
        }
        if (*(*(copyGrid + rowCheck) + colCheck) == 'Q') {
            for (int i = 0; i < m; i++) {
                free(*(copyGrid + i));
            }
            free(copyGrid);
            return false;
        }
    }
    // check mid to bottem left
    rowCheck = row;
    colCheck = column;
    while(rowCheck != m && colCheck != 0) {
        rowCheck++;
        colCheck--;
        if (rowCheck >= m) {
            break;
        }
        if (colCheck < 0) {
            break;
        }
        if (*(*(copyGrid + rowCheck) + colCheck) == 'Q') {
            for (int i = 0; i < m; i++) {
                free(*(copyGrid + i));
            }
            free(copyGrid);
            return false;
        }
    }
    // check mid to top right
    rowCheck = row;
    colCheck = column;
    while(rowCheck != 0 && colCheck != n) {
        rowCheck--;
        colCheck++;
        if (rowCheck < 0) {
            break;
        }
        if (colCheck >= n) {
            break;
        }
        if (*(*(copyGrid + rowCheck) + colCheck) == 'Q') {
            for (int i = 0; i < m; i++) {
                free(*(copyGrid + i));
            }
            free(copyGrid);
            return false;
        }
    }
    for (int i = 0; i < m; i++) {
    free(*(copyGrid + i));
    }
    free(copyGrid);
    return true;

}
//#ifndef QUIET

int SolutionFinder(char** grid, int row, int m, int n, int* pipefd, int numQueens){
    if (row == m) {
        printf("P%d: Found a solution; notifying top-level parent\n", getpid());
        //Notify the parent process through the pipe
        int ArbitraryNum = -5;

        int r;
        close(*(pipefd+0));  // Close the read end of the pipe
        r = write(*(pipefd+1), &ArbitraryNum, sizeof(ArbitraryNum));
        //printf("AWDHAWIDUHAWDIUHAWD %d\n", r);
        //close(*(pipefd+1 ));  // Close the write end of the pipe
        //exit(EXIT_SUCCESS);
        return ArbitraryNum;
    }

    int numPlacements = 0;
    for (int i = 0; i < n; i++) {
        if (checkQueenPlacement(grid, row, i, m, n)) {
            numPlacements++;
        }
    }
    if (numPlacements > 0) {
        printf("P%d: %d possible moves at row #%d; creating %d child processes...\n", getpid(), numPlacements, row, numPlacements);
    }
    else if (numPlacements == 0){
        printf("P%d: Dead end at row #%d\n", getpid(), row);
        

        close(*(pipefd+0));  // Close the read end of the pipe
        int r;
        r = write(*(pipefd+1), &numQueens, sizeof(numQueens));
        //printf("OTHER BASE CASE TEST %d\n", r);
        //close(*(pipefd+1));  // Close the write end of the pipe
        //exit(EXIT_SUCCESS);
        return numQueens;
    }

    for (int i = 0; i < n; i++) {
        if (checkQueenPlacement(grid, row, i, m, n)) {
            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {
                *(*(grid + row) + i) = 'Q';
                numQueens++;
                numQueens = SolutionFinder(grid, row + 1, m, n, pipefd, numQueens);
                exit(numQueens);
            }
        }
    }
}

int main(int argc, char **argv)
{
    fflush(stdout);

    if (argc != 3) {
        fprintf(stderr, "ERROR: Invalid argument(s)\n");
        fprintf(stderr, "USAGE: hw2.out <m> <n>\n");
        return EXIT_FAILURE;
    }

    int m = atoi(*(++argv)); // rows
    int n = atoi(*(++argv)); // columns

    


    printf("P%d: Solving the (%d,%d)-Queens problem for a %dx%d board\n", getpid(), m, n, m, n);
    
    // create grid
    char** grid = (char**)calloc(m, sizeof(char*));
    for (int i = 0; i < m; i++) {
        *(grid+i) = (char*)calloc(n, sizeof(char));
    }

    int* pipefd = (int*)calloc(2, sizeof(int));

    /* create a pipe */
    int rc = pipe( pipefd );
    if ( rc == -1 )
    {
    perror( "pipe() failed" );
    return EXIT_FAILURE;
    }

    bool solutionFound = false;
    int maxQueens = 0;
    int total = 0;
    int solutions = 0;
    for (int i = 0; i < n; i++) {
        if (checkQueenPlacement(grid, 0, i, m, n)) {
            solutions++;
        }
    }
    
    if (solutions > 0) {
        printf("P%d: %d possible moves at row #0; creating %d child processes...\n", getpid(), solutions, solutions);
        for (int i = 0; i < n; i++) {
            if (checkQueenPlacement(grid, 0, i, m, n)) {
                pid_t child_pid = fork();
                if (child_pid == -1)
                {
                    perror("fork() failed");
                    return EXIT_FAILURE;
                }
                if (child_pid == 0) {       // Child Process
                    // place Q down
                    *(*(grid + 0) + i) = 'Q';
                    maxQueens = SolutionFinder(grid, 0 + 1, m, n, pipefd, 1);
                    exit(maxQueens);
                }
                else {                      // Parent Process
                    int status;
                    waitpid(child_pid, &status, 0);
                    if (WIFEXITED(status)) {
                        int childExitStatus = WEXITSTATUS(status);
                        //printf("checker %d\n", childExitStatus);
                        int numQueens;  // if numQueens = -1, then a solution is found, else, it holds the num of queens
                        
                        //close(pipefd[1]);  // Close the write end of the pipe

                        // Read from the pipe
                        ssize_t bytesRead = read(pipefd[0], &numQueens, sizeof(numQueens));

                        if (bytesRead == -1) {
                            perror("read");
                            // Handle the error
                        } else if (bytesRead == 0) {
                            // The write end of the pipe was closed, indicating the end of data
                            //printf("No data available in the pipe.\n");
                        } else {
                            // Successfully read data from the pipe
                            //printf("Read %zd bytes from the pipe. numQueens = %d\n", bytesRead, numQueens);
                        }
                        if (numQueens == -5) {
                            printf("ACCESSED\n");
                            total++;
                            solutionFound = true;
                        }
                        else {
                            if (childExitStatus > maxQueens) {
                                maxQueens = numQueens;
                            }
                        }
                    }
                }

            }
                
        }
    
    }
    
    
    



    else {
        printf("P%d: Dead end at row #0\n", getpid());
    }
    if (solutionFound == false) {
        printf("P%d: Search complete; only able to place %d Queens on a %dx%d board\n", getpid(), maxQueens, m, n);
    }
    else if (solutionFound == true) {
        printf("P%d: Search complete; found %d solutions for a %dx%d board\n", getpid(), total, m, n);
    }

    close(pipefd[1]);

    for (int i = 0; i < m; i++) {
        free(*(grid+i));
    }
    free(grid);
    free(pipefd);
}

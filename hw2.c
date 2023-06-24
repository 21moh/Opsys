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
    		return false;
    	}
    }
    // check for attackers horizontally ---

    for (int j = 0; j < n; j++) {
    	if (*(*(copyGrid + row) + j) == 'Q') {
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
    		return false;
    	}
    }
    return true;

}

void SolutionFinder(char** grid, int row, int m, int n, int solutions, int pipefd[2]){
    if (row == m) {
        solutions++; 
        printf("P%d: Found a solution; notifying top-level parent\n", getpid());
        // Notify the parent process through the pipe
        close(pipefd[0]);  // Close the read end of the pipe
        write(pipefd[1], solutions, sizeof(int));
        close(pipefd[1]);  // Close the write end of the pipe
        exit(EXIT_SUCCESS);
    }
    for (int i = 0; i < n; i++) {
        if (checkQueenPlacement(grid, row, i, m, n)) {
            printf("P%d: 1 possible move at row #%d; creating 1 child process...\n", getpid(), row);
            pid_t child_pid = fork();
            if (child_pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            } else if (child_pid == 0) {
                *(*(grid + row) + i) = 'Q';
                SolutionFinder(grid, row + 1, m, n, solutions, pipefd);
                exit(EXIT_SUCCESS);
            }
        }
    }

    // dead end spot
    printf("P%d: Dead end at row #%d\n", getpid(), row);
    exit(EXIT_SUCCESS);



}


int main(int argc, char **argv)
{
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

	int* pipefd = (int*)malloc(2 * sizeof(int));

	if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Dynamic array to store child process IDs
    pid_t* child_pids = NULL;
    int child_count = 0;

    // the issue with solving it this way is that for second and third row, this problem
    // will solve for the second row with an empty first row

    // What are the safe guards I can put in place?

    // Easier way: recursion until solution reached, or no solution reached

    int solutions = 0;
    SolutionFinder(grid, 0, m, n, solutions, pipefd);



    







    for (int i = 0; i < m; i++) {
        free(*(grid+i));
    }
    free(grid);
    free(pipefd);
}

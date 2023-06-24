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
    		*(*(copyGrid + i) + j) = *(*(grid + i) + j)
    	}
    }
    *(*(copyGrid + row) + column) = 'Q';

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
    	if (rowCheck > m) {
    		break;
    	}
    	if (colCheck > n) {
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
    	if (rowCheck > m) {
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
    	if (colCheck > n) {
    		break;
    	}
    	if (*(*(copyGrid + rowCheck) + colCheck) == 'Q') {
    		return false;
    	}
    }
    return true;

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



    for (int i = 0; i < m; i++) {
    	for (int j = 0; j < n; j++) {
    		if (checkQueenPlacement(grid, i, j, m, n)) {
                pid_t child_pid = fork();
                if (child_pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                } else if (child_pid == 0) {
                    *(*(grid + i) + j) = 'Q';
                    // Child process logic
                    // ...
                    exit(EXIT_SUCCESS);
                } else {
                    // Parent process
                    child_count++;
                    child_pids = (pid_t*)realloc(child_pids, child_count * sizeof(pid_t));
                    child_pids[child_count - 1] = child_pid;
                }
            }
    	}
    }

    // Parent process waits for all child processes to complete
    for (int i = 0; i < child_count; i++) {
        int status;
        pid_t child_pid = waitpid(child_pids[i], &status, 0);
        if (child_pid == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            // Process the exit status of the child if needed
        }
    }







    for (int i = 0; i < m; i++) {
        free(*(grid+i));
    }
    free(grid);
    free(pipfd);
}
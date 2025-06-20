#include <stdio.h>     // printf()
#include <stdlib.h>    // malloc(), exit()
#include <pthread.h>   // Pthreads

int sudoku_grid[9][9] = {
        {5,3,4,6,7,8,9,1,2},
        {6,7,2,1,9,5,3,4,8},
        {1,9,8,3,4,2,5,6,7},
        {8,5,9,7,6,1,4,2,3},
        {4,2,6,8,5,3,7,9,1},
        {7,1,3,9,2,4,8,5,6},
        {9,6,1,5,3,7,2,8,4},
        {2,8,7,4,1,9,6,3,5},
        {3,4,5,2,8,6,1,7,9}
};

typedef struct {
    int row;
    int column;
} parameters;

int results[27] = {0};  // 9 rows + 9 cols + 9 boxes


// Validates a single row of the sudoku grid
void *check_row(void *param) {

    parameters *p = (parameters *) param;
    int *valid = malloc(sizeof(int));   // Result being returned
    int seen[10] = {0};     // Tracks numbers already used

    // Loop through the row, check duplicates & invalid values
    for (int i = 0; i < 9; i++) {
        int num = sudoku_grid[p->row][i];
        if (num < 1 || num > 9 || seen[num]) {
            *valid = 0;     // Invalid row
            pthread_exit(valid);
        }
        seen[num] = 1;      
    }

    *valid = 1; // Valid row
    pthread_exit(valid);
}


int main()
{
    pthread_t row_threads[9]; // Threads to validate rows

    // Loop through grid and create threads
    for (int i = 0; i < 9; i++) {
        
        // Allocate memory and specify parameters for validation
        parameters *data = malloc(sizeof(parameters));
        data->row = i;
        data->column = 0;  // Unused, required by struct

        pthread_create(&row_threads[i], NULL, check_row, data);
    }

    // Join and collect results
    for (int i = 0; i < 9; i++) {
        void *result;
        pthread_join(row_threads[i], &result);
        int *valid_ptr = (int *) result;
        results[i] = *valid_ptr;
        free(valid_ptr);  // Free result
    }

    // Print results
    for (int i = 0; i < 9; i++) {
        printf("Row %d is %s.\n", i, results[i] == 1 ? "valid" : "invalid");
    }

    return 0;
}

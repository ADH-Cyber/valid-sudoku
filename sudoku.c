#include <stdio.h>     // For printf()
#include <stdlib.h>    // For malloc() and exit()
#include <pthread.h>   // For Pthreads

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
    pthread_t thread;  // Thread to validate a row

    // Allocate memory 
    parameters *data = malloc(sizeof(parameters));

    //Initialize parameters to specify which row to check
    data->row = 0;
    data->column = 0;  // Not used for row checking, but required by struct

    void *result;

    // Create and launch the thread to check row 0
    pthread_create(&thread, NULL, check_row, data);

    // Wait for the thread to finish and collect its result
    pthread_join(thread, &result);

    // Cast and evaluate the result (1 = valid, 0 = invalid)
    int *valid_ptr = (int *) result;
    if (*valid_ptr == 1)
        printf("Row 0 is valid.\n");
    else
        printf("Row 0 is invalid.\n");

    // Clean up allocated memory
    free(valid_ptr);  
    free(data);       

    return 0;
}

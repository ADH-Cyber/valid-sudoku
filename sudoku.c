#include <stdio.h>     // printf()
#include <stdlib.h>    // malloc(), exit()
#include <pthread.h>   // Pthreads,

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


// Validates a single column of the sudoku grid
void *check_column(void *param) {

    parameters *p = (parameters *) param;
    int *valid = malloc(sizeof(int));   // Result being returned
    int seen[10] = {0};     // Tracks numbers already used

    // Loop through the column, check duplicates & invalid values
    for (int i = 0; i < 9; i++) {
        int num = sudoku_grid[i][p->column];
        if (num < 1 || num > 9 || seen[num]) {
            *valid = 0;     // Invalid column
            pthread_exit(valid);
        }
        seen[num] = 1;      
    }

    *valid = 1; // Valid column
    pthread_exit(valid);
}


int main()
{
    pthread_t row_threads[9];     // Threads to validate rows
    pthread_t col_threads[9];     // Threads to validate columns

    // Check each row
    for (int i = 0; i < 9; i++) {
        parameters *data = malloc(sizeof(parameters));
        data->row = i;
        data->column = 0;  // Not used
        pthread_create(&row_threads[i], NULL, check_row, data);
    }

    // Store the results of each row
    for (int i = 0; i < 9; i++) {
        void *result;
        pthread_join(row_threads[i], &result);
        int *valid_ptr = (int *) result;
        results[i] = *valid_ptr;         // Store in results[0–8]
        free(valid_ptr);
    }

    // Check each column
    for (int i = 0; i < 9; i++) {
        parameters *data = malloc(sizeof(parameters));
        data->row = 0;                    // Not used
        data->column = i;
        pthread_create(&col_threads[i], NULL, check_column, data);
    }

    // Store the results of each column
    for (int i = 0; i < 9; i++) {
        void *result;
        pthread_join(col_threads[i], &result);
        int *valid_ptr = (int *) result;
        results[i + 9] = *valid_ptr;      // Store in results[9–17]
        free(valid_ptr);
    }

    // Print results
    for (int i = 0; i < 9; i++) {
        printf("Row %d is %s.\n", i, results[i] == 1 ? "valid" : "invalid");
    }

    for (int i = 0; i < 9; i++) {
        printf("Column %d is %s.\n", i, results[i + 9] == 1 ? "valid" : "invalid");
    }

    return 0;
}


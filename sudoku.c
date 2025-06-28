#include <stdio.h>     // printf()
#include <stdlib.h>    // malloc(), exit()
#include <pthread.h>   // Pthreads,

// The grid being used for comparison
int grid[9][9] = {
        {5,3,4,6,7,8,9,1,2},
        {6,7,2,1,9,5,3,4,8},
        {1,9,8,3,4,2,5,6,7},
        {8,5,1,7,6,1,4,2,3},
        {4,2,6,8,5,3,7,9,1},
        {7,1,3,9,2,4,8,5,6},
        {9,6,1,5,3,7,2,8,4},
        {2,8,7,4,1,9,6,3,5},
        {3,4,5,2,8,6,1,7,9}
};

// Define the starting position for comparison
typedef struct {
    int row;
    int column;
} parameters;

// Results array
int results[27] = {0};                  // 9 rows + 9 cols + 9 boxes


// Validates a single row of the sudoku grid
void *check_row(void *param) {

    parameters *p = (parameters *) param;
    int *valid = malloc(sizeof(int));   // Result being returned
    int seen[10] = {0};                 // Tracks numbers already used

    // Loop through the row, check duplicates & invalid values
    for (int i = 0; i < 9; i++) {
        int num = grid[p->row][i];
        if (num < 1 || num > 9 || seen[num]) {
            *valid = 0;                 // Invalid row
            free(param); 
            pthread_exit(valid);
        }
        seen[num] = 1;      
    }

    *valid = 1;                         // Valid row
    free(param); 
    pthread_exit(valid);
}


// Validates a single column of the sudoku grid
void *check_column(void *param) {
    parameters *p = (parameters *) param;
    int *valid = malloc(sizeof(int));   // Result being returned
    int seen[10] = {0};                 // Tracks numbers already used

    // Loop through the column, check duplicates & invalid values
    for (int i = 0; i < 9; i++) {
        int num = grid[i][p->column];
        if (num < 1 || num > 9 || seen[num]) {
            *valid = 0;                 // Invalid column
            free(param); 
            pthread_exit(valid);
        }
        seen[num] = 1;
    }

    *valid = 1;                         // Valid column
    free(param);  
    pthread_exit(valid);
}



// Validates a 3x3 subgrid starting at (row, column)
void *check_subgrid(void *param) {

    parameters *p = (parameters *) param;
    int *valid = malloc(sizeof(int));
    int seen[10] = {0};                 // Track digits 1–9

    // Loop over the 3x3 subgrid
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = grid[p->row + i][p->column + j];
            if (num < 1 || num > 9 || seen[num]) {
                *valid = 0;             // Subgrid is invalid
                free(param); 
                pthread_exit(valid);
            }
            seen[num] = 1;
        }
    }

    *valid = 1;                         // Subgrid is valid
    free(param); 
    pthread_exit(valid);
}


int main()
{
    // Print evaluation grid with borders
    printf("Sudoku Grid:\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%d ", grid[i][j]);
            if ((j + 1) % 3 == 0 && j < 8) printf("| ");
        }
        printf("\n");
        if ((i + 1) % 3 == 0 && i < 8) printf("---------------------\n");
    }
    printf("\n");

    // Create threads
    pthread_t row_threads[9];     
    pthread_t col_threads[9];     
    pthread_t subgrid_threads[9];

    // Check each row
    for (int i = 0; i < 9; i++) {
        parameters *data = malloc(sizeof(parameters));
        data->row = i;
        data->column = 0;               // Not used
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

    // Check each 3x3 subgrid
    int subgrid_idx = 0;
    for (int i = 0; i < 9; i += 3) {
        for (int j = 0; j < 9; j += 3) {
            parameters *data = malloc(sizeof(parameters));
            data->row = i;
            data->column = j;
            pthread_create(&subgrid_threads[subgrid_idx], NULL, check_subgrid, data);
            subgrid_idx++;
        }
    }

    // Join subgrid threads and store in results[18–26]
    for (int i = 0; i < 9; i++) {
        void *result;
        pthread_join(subgrid_threads[i], &result);
        int *valid_ptr = (int *) result;
        results[i + 18] = *valid_ptr;
        free(valid_ptr);
    }

    // Print results
    for (int i = 0; i < 9; i++) {
        printf("Row %d is %s.\n", i, results[i] == 1 ? "valid" : "invalid");
    }

    for (int i = 0; i < 9; i++) {
        printf("Column %d is %s.\n", i, results[i + 9] == 1 ? "valid" : "invalid");
    }

    for (int i = 0; i < 9; i++) {
        printf("Subgrid %d is %s.\n", i, results[i + 18] == 1 ? "valid" : "invalid");
    }

    int all_valid = 1;
    for (int i = 0; i < 27; i++) {
        if (results[i] == 0) {
            all_valid = 0;
            break;
        }
    }

    if (all_valid)
        printf("\n[RESULT:VALID] - Sudoku puzzle is valid!\n");
    else
        printf("\n[RESULTS:INVALID] - Sudoku puzzle is invalid!\n");

    // --- DEBBUGGING - Block comment if unneeded
    // Ensures that valid is returning a valid integer
    printf("\nDEBUG: result bits = ");
    for (int i = 0; i < 27; i++) {
        printf("%d", results[i]);
    }
    printf("\n");


    return 0;
}


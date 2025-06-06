#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_HINTS 3


int isValid(int number, int puzzle[][9], int row, int col);
int count_empty_cells(int puzzle[][9]);
void print_board(int board[][9]);
int solve(int puzzle[][9], int pos);
void copy_board(int src[][9], int dst[][9]);
int generate_puzzle(int puzzle[][9], int difficulty);
int provide_hint(int puzzle[][9], int solved_puzzle[][9]);
int handle_input(int puzzle[][9], int solved_puzzle[][9]);
int is_complete(int puzzle[][9]);
void play_game(int puzzle[][9]);

int isValid(int number, int puzzle[][9], int row, int col) {
    int rowStart = (row / 3) * 3;
    int colStart = (col / 3) * 3;

    for (int i = 0; i < 9; i++) {
        if (puzzle[row][i] == number) return 0;
        if (puzzle[i][col] == number) return 0;
        if (puzzle[rowStart + (i / 3)][colStart + (i % 3)] == number) return 0;
    }
    return 1;
}

int count_empty_cells(int puzzle[][9]) {
    int empty_count = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] == 0) {
                empty_count++;
            }
        }
    }
    return empty_count;
}

void print_board(int board[][9]) {
    printf("\n +-------+-------+-------+\n");
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (j % 3 == 0) {
                printf(" | ");
            } else {
                printf(" ");
            }
            if (board[i][j] == 0) {
                printf("_");
            } else {
                printf("%d", board[i][j]);
            }
        }
        printf(" |\n");
        if (i % 3 == 2) {
            printf(" +-------+-------+-------+\n");
        }
    }
}

int solve(int puzzle[][9], int pos) {
    if (pos == 81) {
        return 1;
    }

    int row = pos / 9;
    int col = pos % 9;

    if (puzzle[row][col] != 0) {
        return solve(puzzle, pos + 1);
    }

    for (int num = 1; num <= 9; num++) {
        if (isValid(num, puzzle, row, col)) {
            puzzle[row][col] = num;
            if (solve(puzzle, pos + 1)) {
                return 1;
            }
            puzzle[row][col] = 0;
        }
    }
    return 0;
}

void copy_board(int src[][9], int dst[][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            dst[i][j] = src[i][j];
        }
    }
}

int generate_puzzle(int puzzle[][9], int difficulty) {
    srand(time(NULL));
    int temp[9][9] = {0};
    
    // Fill diagonal 3x3 blocks (independent, no conflict)
    for (int box = 0; box < 9; box += 3) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                int num;
                do {
                    num = rand() % 9 + 1;
                } while (!isValid(num, temp, box + i, box + j));
                temp[box + i][box + j] = num;
            }
        }
    }

    // Solve to get a complete valid board
    solve(temp, 0);
    copy_board(temp, puzzle);

    // Remove numbers based on difficulty
    int cells_to_remove;
    switch (difficulty) {
        case 1: cells_to_remove = 30; break; // Easy
        case 2: cells_to_remove = 40; break; // Medium
        case 3: cells_to_remove = 50; break; // Hard
        default: cells_to_remove = 40; break;
    }

    while (cells_to_remove > 0) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (puzzle[row][col] != 0) {
            int backup = puzzle[row][col];
            puzzle[row][col] = 0;

            // Check if the puzzle still has a unique solution
            int temp_puzzle[9][9];
            copy_board(puzzle, temp_puzzle);
            int solutions = 0;
            int temp_pos = 0;
            while (temp_pos < 81 && solutions <= 1) {
                if (solve(temp_puzzle, temp_pos)) {
                    solutions++;
                    copy_board(puzzle, temp_puzzle); // Reset for next check
                }
                temp_pos = 81; // Stop after first solution
            }

            if (solutions != 1) {
                puzzle[row][col] = backup; // Restore if not unique
            } else {
                cells_to_remove--;
            }
        }
    }
    return 1;
}

int provide_hint(int puzzle[][9], int solved_puzzle[][9]) {
    static int hints_used = 0;
    if (hints_used >= MAX_HINTS) {
        printf("No more hints available! (Max: %d)\n", MAX_HINTS);
        return 0;
    }

    // Find a random empty cell
    int empty_cells[81][2];
    int empty_count = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] == 0) {
                empty_cells[empty_count][0] = i;
                empty_cells[empty_count][1] = j;
                empty_count++;
            }
        }
    }

    if (empty_count == 0) {
        printf("No empty cells to provide a hint!\n");
        return 0;
    }

    int idx = rand() % empty_count;
    int row = empty_cells[idx][0];
    int col = empty_cells[idx][1];
    puzzle[row][col] = solved_puzzle[row][col];
    hints_used++;
    printf("Hint: Placed %d at row %d, column %d. Hints remaining: %d\n", 
           puzzle[row][col], row + 1, col + 1, MAX_HINTS - hints_used);
    print_board(puzzle);
    return 1;
}

int handle_input(int puzzle[][9], int solved_puzzle[][9]) {
    int row, col, num;
    printf("Enter row (1-9), column (1-9), and number (1-9) to fill, -1 -1 -1 for a hint, or 0 0 0 to solve: ");
    scanf("%d %d %d", &row, &col, &num);

    if (row == -1 && col == -1 && num == -1) {
        return provide_hint(puzzle, solved_puzzle);
    }

    if (row == 0 && col == 0 && num == 0) {
        return 0; // Signal to solve
    }

    if (row < 1 || row > 9 || col < 1 || col > 9 || num < 1 || num > 9) {
        printf("Invalid input! Enter values between 1-9.\n");
        return -1;
    }

    row--; col--; // Convert to 0-based indexing

    if (puzzle[row][col] != 0) {
        printf("Cell already filled! Try another.\n");
        return -1;
    }

    if (isValid(num, puzzle, row, col)) {
        puzzle[row][col] = num;
        printf("Cell updated successfully!\n");
        print_board(puzzle);
    } else {
        printf("Invalid number for this cell! Try again.\n");
        return -1;
    }
    return 1;
}

int is_complete(int puzzle[][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (puzzle[i][j] == 0) {
                return 0; // Not complete
            }
        }
    }
    return 1; // Complete
}

void play_game(int puzzle[][9]) {
    int solved_puzzle[9][9];
    copy_board(puzzle, solved_puzzle);
    solve(solved_puzzle, 0); // Precompute solution for hints

    printf("\n=== Sudoku Game ===\n");
    printf("Enter row, column, and number (1-9) to fill, -1 -1 -1 for a hint, or 0 0 0 to solve.\n");
    printf("You have %d hints available.\n", MAX_HINTS);
    print_board(puzzle);

    int result;
    while (1) {
        result = handle_input(puzzle, solved_puzzle);
        if (result == 0) {
            break; // Solve the puzzle
        }
        if (is_complete(puzzle)) {
            printf("\nPuzzle completed manually!\n");
            print_board(puzzle);
            break;
        }
    }

    if (result == 0) {
        copy_board(solved_puzzle, puzzle);
        printf("\nSudoku Solved Successfully!\n");
        print_board(puzzle);
    }
}

int main() {
    int sudoku_puzzle[9][9] = {0};
    int difficulty;

    printf("Choose difficulty (1: Easy, 2: Medium, 3: Hard): ");
    scanf("%d", &difficulty);
    if (difficulty < 1 || difficulty > 3) {
        printf("Invalid difficulty! Defaulting to Medium.\n");
        difficulty = 2;
    }

    srand(time(NULL));
    generate_puzzle(sudoku_puzzle, difficulty);
    printf("Generated Sudoku Puzzle:\n");
    print_board(sudoku_puzzle);

    play_game(sudoku_puzzle);

    return 0;
}

#include <stdbool.h>
#include <stdint.h>
typedef uint8_t u8;

#define panic(msg, ...) _panic(__FILE__, __LINE__, __func__, msg, __VA_ARGS__)
void _panic(const char* file, int line, const char* func, const char* msg, ...);

u8 find_zeros(u8 *grid, u8 *zeros) {
    u8 index = 0;
    for (u8 i = 0; i < 81; i++) {
        if (grid[i] == 0) {
            zeros[index] = i;
            index++;
        }
    }
    return index;
}

bool is_valid(u8 *grid, u8 num, u8 index) {
    if (!(1 <= num && num < 10)) {
        panic("is_valid: Argument error: Number out of range: %d\n", num);
        return false;
    }
    u8 column = index % 9;
    u8 row = index / 9;
    u8 square_col = column / 3;
    u8 square_row = index / 27;
    u8 tmp = grid[index];
    grid[index] = 0;
    // check row
    for (u8 i = row * 9; i < (row + 1) * 9; i++) {
        if (grid[i] == num) {
            grid[index] = tmp;
            return false;
        }
    }
    // check column
    for (u8 i = column; i < 81; i += 9) {
        if (grid[i] == num) {
            grid[index] = tmp;
            return false;
        }
    }
    // check square
    for (
        u8 i = square_row * 27 + square_col * 3;
        i < (square_row + 1) * 27 + square_col * 3;
        i += 9) {
        for (u8 j = 0; j < 3; j++) {
            if (grid[i + j] == num) {
                grid[index] = tmp;
                return false;
            }
        }
    }
    grid[index] = tmp;
    return true;
}

typedef enum {
    SR_UNSOLVABLE = 0,
    SR_SOLVED = 10,
} SolveResult;

SolveResult solve(u8 *grid) {
    u8 zeros[82] = {0};
    u8 length = find_zeros(grid, zeros);
    for (u8 i = 0; i < 81; i++) {
        if (grid[i] == 0) {
            continue;
        }
        else if (!is_valid(grid, grid[i], i)) {
            panic("Initial solution is wrong %d", 0);
            return SR_UNSOLVABLE;
        }
        else {
        }
    }
    u8 grid_index = 0;
    u8 zeros_index = 0;
    for (;;) {
        u8 num = 0;
        if (!(0 <= zeros_index && zeros_index < length)) {
            break;
        } else {
            grid_index = zeros[zeros_index];
        }
        if (!(0 <= grid_index && grid_index < 81)) {
            panic("Grid index in zero index wrong %d", grid_index);
            return SR_UNSOLVABLE;
        } else {
            num = grid[grid_index] + 1;
        }
        for (;;) {
            //printf("try index %d num %d\n", grid_index, num);
            if (num == 10) {
                // No valid num found -> backtrack
                grid[grid_index] = 0;
                //printf("back index %d num %d\n", grid_index, num);
                if (zeros_index == 0) {
                    return SR_UNSOLVABLE;
                } else {
                    zeros_index--;
                    break;
                }
            }
            if (is_valid(grid, num, grid_index)) {
                grid[grid_index] = num;
                //printf("forward index %d num %d\n", grid_index, num);
                zeros_index++;
                break;
            }
            num++;
        }
    }
    //printf("Validating...");
    for (u8 x = 0; x < 81; x++) {
        if (!is_valid(grid, grid[x], x)) {
            panic("Initial solution is wrong %d", 0);
            return SR_UNSOLVABLE;
        }
    }
    return SR_SOLVED;
}

#ifdef __wasm32__
#include <stddef.h>
void *memset(void *ptrv, int x, size_t n) {
    char* ptr = (char*)ptrv;
    for (size_t i = 0; i < n; i++) {
        ptr[i] = x;
    }
    return ptr;
}
void console_panic(void);

void _panic(const char* file, int line, const char* func, const char* msg, ...) {
    console_panic();
}

u8 grid[81] = {0};

void set_value(u8 loc, u8 val) {
    if (0 <= loc && loc < 81) {
        if (0 <= val && val <= 9) {
            grid[loc] = val;
        }
        else {
            panic("Invalid set at loc %d with invalid value %d", loc, val);
        }
    }
    else {
        panic("Invalid set at loc %d", loc);
    }
}


#else
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
void _panic(const char* file, int line, const char* func, const char* msg, ...) {
    va_list ap;
    printf("%s:%s:%d: ", file, func, line);
    va_start(ap, msg);
    vprintf(msg, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

void print_grid(u8 *grid) {
    for (u8 i = 0; i < 81; i++) {
        if (i % 27 == 0 && i != 0) {
            printf("\n------+-------+------\n");
        } else if (i % 9 == 0) {
            printf("\n");
        } else if (i % 3 == 0) {
            printf("| ");
        }
        printf("%d ", grid[i]);
    }
    printf("\n\n");
}

void print_array(u8 *arr, u8 length) {
    printf("[");
    for (u8 i = 0; i < length - 1; i++) {
        printf("%d, ", arr[i]);
    }
    printf("%d]\n", arr[length - 1]);
}
int main(void) {
    /*u8 grid[81] = {
        0, 0, 0, 0, 0, 4, 0, 0, 0,
        3, 0, 5, 0, 0, 0, 4, 0, 1,
        0, 1, 4, 2, 6, 0, 0, 0, 0,
        0, 8, 3, 4, 0, 0, 0, 0, 0,
        2, 9, 0, 1, 0, 6, 0, 4, 3,
        0, 0, 0, 0, 0, 5, 9, 8, 0,
        0, 0, 0, 0, 2, 1, 3, 9, 0,
        9, 0, 1, 0, 0, 0, 8, 0, 4,
        0, 0, 0, 9, 0, 0, 0, 0, 0};*/
    u8 grid[81] = {
        1, 2, 3, 4, 5, 6, 7, 8, 9, 4, 5, 6, 7, 8, 9, 1, 2, 3, 7, 8, 9, 1, 2, 3, 4, 5, 6, 2, 1, 4, 3, 6, 5, 8, 9, 7, 3, 6, 5, 8, 9, 7, 2, 1, 4, 8, 9, 7, 2, 1, 4, 3, 6, 5, 5, 3, 1, 6, 4, 2, 9, 7, 8, 6, 4, 2, 9, 7, 8, 5, 3, 1, 9, 7, 8, 5, 3, 1, 0, 9, 2
    };
    print_grid(grid);
    if (solve(grid) == SR_SOLVED) {
        print_grid(grid);
    }
    else {
        printf("Unsolvable");
    }
    return 0;
}
#endif

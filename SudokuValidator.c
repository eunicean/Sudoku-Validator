#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <omp.h>

#define SIZE 9

int sudoku[9][9];

// Función para verificar si una fila contiene los números 1-9
void* validar_filas(void* arg) {
    for (int i = 0; i < SIZE; i++) {
        int check[SIZE] = {0};
        for (int j = 0; j < SIZE; j++) {
            if (check[sudoku[i][j] - 1] == 1) return (void*)0;
            check[sudoku[i][j] - 1] = 1;
        }
    }
    return (void*)1;
}

// Función para verificar si una columna contiene los números 1-9
void* validar_columnas(void* arg) {
    printf("Thread de columnas ID: %ld\n", syscall(SYS_gettid));
    for (int j = 0; j < SIZE; j++) {
        int check[SIZE] = {0};
        for (int i = 0; i < SIZE; i++) {
            if (check[sudoku[i][j] - 1] == 1) return (void*)0;
            check[sudoku[i][j] - 1] = 1;
        }
    }
    return (void*)1;
}

// Función para verificar una submatriz 3x3
int validar_submatriz(int row, int col) {
    int check[SIZE] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[row + i][col + j];
            if (check[num - 1] == 1) return 0;
            check[num - 1] = 1;
        }
    }
    return 1;
}



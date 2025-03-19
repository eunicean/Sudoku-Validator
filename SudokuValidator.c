#include <stdio.h>
#include <stdlib.h>

#define SUDOKU_SIZE 9

int sudoku[SUDOKU_SIZE][SUDOKU_SIZE];

// Funcion para validar si una fila contiene los numeros del 1 al 9
void* validar_filas(void* arg) {
    int i, j, k;
    int* fila = (int*) arg;
    int* numeros = (int*) calloc(SUDOKU_SIZE, sizeof(int));

    for (i = 0; i < SUDOKU_SIZE; i++) {
        k = fila[i];
        if (numeros[k] == 1) {
            return (void*) 0;
        }
        numeros[k] = 1;
    }

    return (void*) 1;
}

// Funcion para validar si una columna contiene los numeros del 1 al 9
void* validar_columnas(void* arg) {
    int i, j, k;
    int* columna = (int*) arg;
    int* numeros = (int*) calloc(SUDOKU_SIZE, sizeof(int));

    for (i = 0; i < SUDOKU_SIZE; i++) {
        k = columna[i];
        if (numeros[k] == 1) {
            return (void*) 0;
        }
        numeros[k] = 1;
    }

    return (void*) 1;
}

// Funcion para validar si un cuadrante contiene los numeros del 1 al 9
void* validar_submatriz(int row, int col){
    int check[SUDOKU_SIZE] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[row + i][col + j];
            if (check[num - 1] == 1) return 0;
            check[num - 1] = 1;
        }
    }
    return 1;
}

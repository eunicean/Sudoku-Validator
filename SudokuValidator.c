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
#include <stdbool.h>

#define SIZE 9

// Matriz global del Sudoku
int sudoku[SIZE][SIZE];

// Función para verificar si una fila contiene los números 1-9
// OpenMP
void* validar_filas() {
    omp_set_num_threads(SIZE);
    omp_set_nested(true);
    
    int *is_valid = malloc(sizeof(int));
    *is_valid = 1;
    int thread_id = syscall(SYS_gettid);
    
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < SIZE; i++) {
        printf("Thread verificando fila %d: %d\n",i, thread_id);
        int check[SIZE] = {0};
        for (int j = 0; j < SIZE; j++) {
            if (check[sudoku[i][j] - 1] == 1) {
                #pragma omp critical
                *is_valid = 0;
            }
            check[sudoku[i][j] - 1] = 1;
        }
    }
    return is_valid;
}

// Función para verificar columnas con OpenMP
void* validar_columnas() {
    omp_set_num_threads(SIZE);
    omp_set_nested(true);

    int *is_valid = malloc(sizeof(int));
    *is_valid = 1;
    int thread_id = syscall(SYS_gettid);
    
    #pragma omp parallel for schedule(dynamic)
    for (int j = 0; j < SIZE; j++) {
        printf("Thread verificando columna %d: %d\n",j, thread_id);
        int check[SIZE] = {0};
        for (int i = 0; i < SIZE; i++) {
            if (check[sudoku[i][j] - 1] == 1) {
                #pragma omp critical
                *is_valid = 0;
            }
            check[sudoku[i][j] - 1] = 1;
        }
    }
    pthread_exit(is_valid);
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

int main(int argc, char const *argv[]) {
    omp_set_num_threads(1);

    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_sudoku>\n", argv[0]);
        return 1;
    }
    
    size_t file_size = SIZE * SIZE;
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error al abrir el archivo");
        return -1;
    }
    
    char *file_memory = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
    if (file_memory == MAP_FAILED) {
        perror("Error en mmap");
        close(fd);
        return -1;
    }
    
    printf("Contenido del archivo: %s\n", file_memory);
    int k = 0;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            sudoku[i][j] = file_memory[k] - '0';
            printf("%d ", sudoku[i][j]);
            k++;    
        }
        printf("\n");
    }
    
    int subgrid_valid = 0;
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            subgrid_valid += validar_submatriz(i, j);
        }
    }
    
    if (subgrid_valid == SIZE) {
        printf("Sudoku válido\n");
    } else {
        printf("Sudoku inválido\n");
    }

    pid_t parent_pid = getpid();
    pid_t child_pid = fork();
    if (child_pid == 0) {
        char buffer[10];
        sprintf(buffer, "%d", parent_pid);
        execlp("ps", "ps", "-p", buffer, "-lLf", NULL);
    }
    wait(NULL);

    pthread_t column_thread;
    int *column_result;
    pthread_create(&column_thread, NULL, validar_columnas, NULL);
    pthread_join(column_thread, (void**)&column_result);
    printf("Proceso principal: %d\n", parent_pid);
    
    int *row_result = validar_filas();
    
    if (*row_result == 1 && *column_result == 1) {
        printf("\nSudoku válido\n");
    } else {
        printf("\nSudoku inválido\n");
    }
    
    pid_t second_child = fork();
    if (second_child == 0) {
        char buffer[10];
        sprintf(buffer, "%d", parent_pid);
        execlp("ps", "ps", "-p", buffer, "-lLf", NULL);
    }
    wait(NULL);
    
    free(column_result);
    free(row_result);
    munmap(file_memory, file_size);
    close(fd);
    return 0;
}

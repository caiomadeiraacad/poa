#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#define N 4
#define M 4

int m[N][M] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
};

int is_occupied[N][M] = {
    false, false, false, false,
    false, false, false, false,
    false, false, false, false,
    false, false, false, false,
};

void printM(int m[N][M]) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < M; j++) {
            printf("%d ", m[i][j]);
        }
        printf("\n");
    }
}

void insert_queen(int is_occupied[N][M], int m[N][M]) {
    int i, j = 0;
    for(i = 0; i < N; i++) {
        for(j = 0; j < M; j++) {
            if (is_occupied[i][j] != true) {
              m[i][j] = 1;
            }
        }
    }
}
/*

Pseudocódigo para algoritmos de backtrackingrecursivo
Se estiver em uma solução, retorne comsucesso
Para cada(escolha possível do estado/nóatual)
Faça essa escolha e dê um passo no caminhoUse recursão para resolver o problema para onovo nó/estado
Se a chamada recursiva for bem-sucedida,relate o sucesso para o próximo nível altoSaia da escolha atual para restaurar oestado no início do loop.
Falha
*/

// O que define uma solucao? uma solucao eh quando, em um tbauleiro NxN tu consegue posicionar as N rianhas de modo q nenhuma ataque a outra

bool is_safe(int line, int column, int m[N][M]) {
    for(int i = 0; i < line - 1; i++) {
        if (m[i][column] == 1) { return false; }
    }

    for(int i = 0; i < N*M; i++) {
        int col = (N*M) - 1 - i;
        if (m[i][col] == 1) { return false; }
    }

    for (int j = 0; j < column - 1; j++) {
        if (m[line][j] == 1) { return false; }
    }

    return true;
}


void nqueens(int current_line, int m[N][M]) {
    if (current_line == N) {
        printf("=======================\n");
        printM(m);
        printf("=======================\n");
        return;
    }
    int i, j = 0;
    for(j = 0; j < M; j++) {
        if (is_safe(current_line, j, m) == true) {
            //is_occupied[current_line][j] = true;
            m[current_line][j] = 1;
            nqueens(current_line + 1, m);
            m[current_line][j] = 0;
        }
    }
}

int main(void) {

    printM(m);
    printf("=============\n");
    nqueens(0, m);

    return 0;
}
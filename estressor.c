#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h> // Necessário para a função getpid()

// Tamanho da matriz: 1000x1000 garante o tempo de processamento necessário
#define SIZE 1000 

/* Função dedicada a calcular e imprimir as métricas exigidas pelo projeto */
void capturar_metricas(struct timeval *inicio, struct timeval *fim) {
    struct rusage uso;
    // Interroga o Kernel sobre o tempo de CPU gasto por este processo
    getrusage(RUSAGE_SELF, &uso);

    // Cálculo matemático do tempo de parede (diferença entre o relógio final e inicial)
    double tempo_parede = (fim->tv_sec - inicio->tv_sec) + 
                          (fim->tv_usec - inicio->tv_usec) * 1e-6;

    printf("\n--- Metricas de Execucao ---\n");
    // Tempo real que se passou no mundo físico (crucial para provar a espera imposta pelo escalonador)
    printf("Tempo de Parede (Wallclock): %.6f s\n", tempo_parede);
    // Tempo que a CPU efetivamente passou fazendo as multiplicações
    printf("Tempo de Usuario (User Time): %ld.%06ld s\n", uso.ru_utime.tv_sec, uso.ru_utime.tv_usec);
    // Tempo que o Kernel gastou resolvendo coisas para este processo
    printf("Tempo de Sistema (Sys Time): %ld.%06ld s\n", uso.ru_stime.tv_sec, uso.ru_stime.tv_usec);
}

int main() {
    // Imprime o PID logo de cara para facilitar a automação com scripts de teste
    printf("Processo Estressor (PID: %d) iniciado.\n", getpid());

    // Alocação Dinâmica na Heap (evita o erro de Stack Overflow causado por matrizes gigantes)
    double *A = malloc(SIZE * SIZE * sizeof(double));
    double *B = malloc(SIZE * SIZE * sizeof(double));
    double *C = malloc(SIZE * SIZE * sizeof(double));

    if (!A || !B || !C) {
        printf("Erro de alocacao de memoria!\n");
        return 1;
    }

    // Preenche as matrizes A e B com dados aleatórios, zera a matriz C
    for(int i = 0; i < SIZE * SIZE; i++) { 
        A[i] = rand(); 
        B[i] = rand(); 
        C[i] = 0;
    }

    // Variáveis para guardar a "foto" do relógio da parede
    struct timeval inicio, fim;
    
    printf("Iniciando calculo intensivo (Multiplicacao %dx%d)...\n", SIZE, SIZE);
    
    // Marca o instante exato ANTES da CPU começar a fritar
    gettimeofday(&inicio, NULL); 

    // O coração do Estressor: Multiplicação de Matrizes com complexidade $O(n^3)$
    for(int i = 0; i < SIZE; i++) {
        for(int j = 0; j < SIZE; j++) {
            for(int k = 0; k < SIZE; k++) {
                C[i * SIZE + j] += A[i * SIZE + k] * B[k * SIZE + j];
            }
        }
    }

    // Marca o instante exato DEPOIS que o cálculo termina
    gettimeofday(&fim, NULL); 

    // Envia os relógios para a função que fará as contas finais e imprimirá na tela
    capturar_metricas(&inicio, &fim);

    // Boas práticas: devolver a memória alocada para o sistema operacional
    free(A); 
    free(B); 
    free(C);
    
    return 0;
}

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

void capturar_metricas() {
    struct rusage uso;
    struct timeval tempo_parede;
    
    gettimeofday(&tempo_parede, NULL);
    getrusage(RUSAGE_SELF, &uso);
    
    long tempo_user_ms = (uso.ru_utime.tv_sec * 1000) + (uso.ru_utime.tv_usec / 1000);
    long tempo_sys_ms = (uso.ru_stime.tv_sec * 1000) + (uso.ru_stime.tv_usec / 1000);
    
    printf("\nMetricas de Execucao (Milissegundos):\n");
    printf("Tempo de Usuario (User Time): %ld ms\n", tempo_user_ms);
    printf("Tempo de Sistema (Sys Time): %ld ms\n", tempo_sys_ms);
}

int main(int argc, char *argv[]) {
    if (argc > 1 && atoi(argv[1]) == 7) {
        struct sched_param p = { .sched_priority = 0 };
        if (sched_setscheduler(0, 7, &p) == 0) {
            printf("PID %d: Iniciado em SCHED_BACKGROUND (Politica 7)\n", getpid());
        } else {
            perror("Erro ao definir SCHED_BACKGROUND. Use sudo!");
            return 1;
        }
    } else {
        printf("PID %d: Iniciado em SCHED_OTHER (Politica Padrao)\n", getpid());
    }

    printf("Executando carga de trabalho intensiva (Calculo em lote)...\n");
    
    volatile double calculo = 0.0;
    for(long i = 0; i < 800000000; i++) {
        calculo += 0.00001; 
    }

    capturar_metricas();
    return 0;
}

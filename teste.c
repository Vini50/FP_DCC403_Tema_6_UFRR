#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

int main() {
    struct sched_param p = { .sched_priority = 0 };
    
    printf("PID %d: Solicitando politica 7 (Background)...\n", getpid());

    if (sched_setscheduler(0, 7, &p) == 0) {
        printf("PID %d: SUCESSO ABSOLUTO! Escalonador aceitou a classe.\n", getpid());
    } else {
        perror("ERRO ao mudar politica");
        return 1;
    }

    printf("PID %d: Iniciando loop infinito leve. O sistema NAO vai travar.\n", getpid());

    unsigned long voltas = 0;
    
    // Loop infinito exigido para a avaliação
    while (1) {
        voltas++;
        
        // Imprime na tela a cada 5 voltas para não poluir demais o terminal
        if (voltas % 5 == 0) {
            printf("PID %d: Executando perfeitamente em Background...\n", getpid());
        }
        
        // Carga de trabalho falsa: Atraso matemático puro
        for(volatile long j = 0; j < 100000000; j++); 
        
        // A MÁGICA: Cede a CPU de volta para o Ubuntu. Evita o travamento total!
        sched_yield(); 
    }

    return 0;
}

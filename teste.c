/*testar se as chamadas de sistema nativas 
sched_setscheduler()  conseguem alterar a classe do processo para a política 7 sem travar a máquina. */

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>

int main() {
    /* Define a prioridade estática como 0, obrigatório para políticas normais/background */
    struct sched_param p = { .sched_priority = 0 };
    
    printf("PID %d: Solicitando politica 7 (SCHED_BACKGROUND)...\n", getpid());

    /* Tenta mudar a política do próprio processo (0) para 7 */
    if (sched_setscheduler(0, 7, &p) == 0) {
        printf("PID %d: SUCESSO! O Kernel aceitou a mudança de classe.\n", getpid());
    } else {
        perror("ERRO ao mudar politica. Voce executou com sudo?");
        return 1;
    }

    printf("PID %d: Entrando em loop infinito para validar estabilidade...\n", getpid());
    
    while (1) {
        for(volatile long j = 0; j < 100000000; j++); 
        sched_yield(); 
    }
    return 0;
}
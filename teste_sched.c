#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Bibliotecas cruciais para interagir com a API de agendamento do Kernel Linux
#include <sched.h>   
#include <errno.h>   
#include <unistd.h>  

// Define a constante exata que mapeamos no Kernel (core.c e valid_policy)
// O número 7 é o identificador exclusivo da sua nova política
#define SCHED_BACKGROUND 7

int main(int argc, char *argv[]) {
    // 1. Validação de Argumentos
    // O programa exige que o usuário passe exatamente um argumento: o PID do processo.
    if (argc != 2) {
        fprintf(stderr, "Uso incorreto.\nSintaxe esperada: %s <PID_DO_PROCESSO>\n", argv[0]);
        exit(1);
    }

    // Converte a string digitada no terminal (ex: "5584") para um número inteiro (pid_t)
    pid_t pid = atoi(argv[1]);
    
    // 2. Configuração dos Parâmetros do Escalonador
    // A estrutura sched_param é obrigatória para a system call.
    struct sched_param param;
    
    // Como a política SCHED_BACKGROUND não é de Tempo Real (Real-Time), 
    // a prioridade estática DEVE ser zero. Se colocar outro valor, o Kernel rejeita.
    param.sched_priority = 0; 

    printf("Tentando aplicar a politica SCHED_BACKGROUND (7) ao processo %d...\n", pid);

    // 3. A Chamada de Sistema (System Call)
    // sched_setscheduler pede ao Kernel para mudar a política do PID especificado.
    // Retorna 0 em caso de sucesso e -1 em caso de erro.
    if (sched_setscheduler(pid, SCHED_BACKGROUND, &param) == -1) {
        // perror traduz o código de erro numérico do Kernel para uma mensagem legível
        perror("FALHA AO DEFINIR POLITICA");
        fprintf(stderr, "Dica: Você usou 'sudo'? Alterar politicas de escalonamento exige privilegios de root.\n");
        return 1;
    }

    // 4. Mecanismo de Validação (Exigência do Projeto)
    // Para provar que o Kernel realmente aceitou (e não apenas ignorou), 
    // interrogamos o Kernel novamente para ver qual política o processo está usando agora.
    int politica_atual = sched_getscheduler(pid);
    
    if (politica_atual == SCHED_BACKGROUND) {
        printf("SUCESSO! O Kernel confirmou: Processo %d agora roda em SCHED_BACKGROUND.\n", pid);
    } else {
        // Se cair aqui, a system call não deu erro, mas o Kernel mapeou para outra classe (ex: CFS).
        printf("AVISO: O comando rodou, mas o processo foi alocado na política %d.\n", politica_atual);
    }

    return 0;
}

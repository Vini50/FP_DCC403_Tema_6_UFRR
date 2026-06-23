# FP_DCC403_Tema_6_UFRR

Este repositório contém os artefatos finais para o desenvolvimento de uma nova política de escalonamento de processos para tarefas em segundo plano no Kernel do Linux. O objetivo principal é projetar, codificar e injetar uma classe nativa denominada SCHED_BACKGROUND.  

Esta política atua como o nível mais baixo na hierarquia de agendamento do sistema operacional. Ela é estritamente não-preemptiva, ou seja, as tarefas nela cadastradas só receberão ciclos de CPU se, e somente se, as filas de prontos das classes de tempo real (SCHED_FIFO, SCHED_RR) e da classe padrão (SCHED_OTHER/SCHED_NORMAL) estiverem completamente vazias no respectivo núcleo do processador. O algoritmo previne a inanição competitiva (starvation) utilizando um modelo de Round-Robin interno baseado em matrizes dimensionadas pelos núcleos (SMP).  
Estrutura do Repositório

Todos os artefatos gerados para este trabalho estão centralizados neste repositório:  

    P_DCC403_Tema_6_UFRR_Patch.patch: Arquivo de patch gerado via git diff contendo as alterações cirúrgicas e a modificação estrutural na árvore de compilação do Kernel (modificações em background.c, core.c e vmlinux.lds.h).  

    teste.c: Mecanismo de validação em linha de comando que modifica a classe de um processo em tempo de execução fazendo uso da chamada nativa sched_setscheduler().  

    benchmark.c: Programa de benchmarking construído para a coleta métrica rigorosa utilizando as chamadas de sistema gettimeofday() e getrusage().  

    Relatorio.pdf: Documentação oficial contendo a fundamentação teórica, guia de engenharia e análise empírica do impacto e isolamento da nova classe. (Insira o PDF do seu relatório aqui ao finalizar).  

Pré-requisitos do Ambiente

    Sistema Operacional base homologado: Ubuntu 24.04 LTS.

    Código-fonte da árvore nativa do Linux (versão 6.8.0).

    Compiladores da linguagem C/C++ ou Rust e ferramentas de build (GCC, Make, bibliotecas do Kbuild).  

Instruções de Instalação e Compilação

Para reproduzir o ambiente, faça o download do código-fonte do Kernel 6.8.0 e siga o pipeline de compilação abaixo:  

    Aplique o arquivo de patch na raiz do código-fonte do Kernel para injetar as modificações estruturais:
    patch -p1 < P_DCC403_Tema_6_UFRR_Patch.patch

    Inicie a compilação paralela da imagem do Kernel desativando a conversão de avisos em erros:
    make WERROR=0 -j4

    Instale os módulos compilados:
    sudo make modules_install

    Gere a nova imagem do Kernel e atualize o gerenciador de inicialização (GRUB):
    sudo make install  

    Reinicie a máquina para inicializar o sistema operacional customizado.  

Instruções de Teste e Validação

Após estabilizar e depurar o Kernel modificado, o utilitário em espaço de usuário deve ser compilado e executado para o ensaio experimental.  

Compile os utilitários de teste fornecidos:

    gcc teste.c -o teste

    gcc benchmark.c -o benchmark

Validando a estabilidade da política 7:

    sudo ./teste

Disparando a matriz de benchmarking (Análise de Sobrecarga):

Para coletar o tempo de relógio de parede e tempo de CPU , garantindo que o impacto no processo prioritário seja próximo a 0%, execute simultaneamente:  

    Processo prioritário (SCHED_OTHER): ./benchmark

    Processo em segundo plano (SCHED_BACKGROUND): sudo ./benchmark 7

Autor: Vinicius (Vini50)
Disciplina: Tópico de Aula - Apresentação do Projeto Final

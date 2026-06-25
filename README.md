# Implementação do Escalonador SCHED_BACKGROUND (Kernel Linux 5.15)

**Disciplina:** Sistemas Operacionais (DCC403)  
**Instituição:** Universidade Federal de Roraima (UFRR)  
**Tema do Projeto:** Tema 6  

Este repositório contém os artefatos finais exigidos para a avaliação do projeto prático de alteração cirúrgica no núcleo (Kernel) do Linux. O objetivo principal foi projetar e implementar uma nova política de escalonamento focada em isolamento de recursos.

## 📌 Visão Geral do Projeto

Foi criada uma nova política de escalonamento nativa no Kernel do Linux denominada **`SCHED_BACKGROUND`** (identificada internamente pelo número 7). 

A motivação por trás desta classe é solucionar o problema de concorrência de recursos em ambientes de servidores. Processos utilitários (como backups, compactação de logs ou rotinas assíncronas) frequentemente roubam tempo de CPU de serviços críticos (como bancos de dados), mesmo quando configurados com baixa prioridade (*niceness* máxima). 

A política `SCHED_BACKGROUND` resolve isso atuando no nível mais baixo da hierarquia matemática do agendador: ela é **estritamente não-preemptiva** e garante que as tarefas de segundo plano só recebam ciclos de processamento se o sistema estiver absolutamente ocioso (posicionada logo acima da classe `idle` e abaixo da `fair`).

## 🛠️ Arquitetura e Modificações no Kernel

Para que o sistema operacional reconhecesse a nova política, foram realizadas alterações estruturais profundas na árvore do Kernel **Vanilla 5.15.137**:

* **`include/linux/sched.h`**: Adição do campo `bg_run_list` na `struct task_struct` para enfileiramento de processos.
* **`kernel/sched/sched.h`**: Criação da `struct bg_rq` e inserção dentro da fila global de execução da CPU (`struct rq`).
* **`kernel/sched/core.c`**: Alterações na inicialização (`sched_init` e `__sched_fork`) para formatação das listas encadeadas (`INIT_LIST_HEAD`), prevenindo *Page Faults* por desreferenciamento nulo no boot.
* **`kernel/sched/background.c`**: Implementação das funções vitais da classe (`enqueue_task`, `dequeue_task`, `pick_next_task`, `task_tick`) com suporte a *Round-Robin* e validações para multiprocessamento simétrico (SMP).
* **`vmlinux.lds.h`**: Modificação no *linker script* para compilar e mapear a classe `*(__background_sched_class)` no endereço correto de memória para a varredura do núcleo.

## 📁 Arquivos do Repositório

* `patch_projeto_final.diff`: O patch oficial contendo o código C extraído via `git diff` com todas as alterações realizadas na árvore do Kernel.
* `teste_sched.c`: Ferramenta de *User Space* que utiliza a *system call* `sched_setscheduler()` para alterar a prioridade de um processo alvo em tempo de execução.
* `estressor.c`: Script de benchmarking e cálculo matemático intensivo para simulação de carga pesada e monitoramento de *User Time* e *System Time*.
* `Relatorio_Final.pdf`: Artigo detalhando a metodologia, o ambiente de desenvolvimento e a prova de isolamento.

## 🚀 Como Aplicar e Testar

O ambiente alvo para este patch é uma máquina virtual (Ubuntu 22.04 LTS) com múltiplos núcleos.

### 1. Aplicando o Patch
Baixe o Kernel original e aplique as modificações:
```bash
wget [https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.137.tar.xz](https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.137.tar.xz)

### 2. Compilação
Bash

make localmodconfig
make -j$(nproc)
sudo make modules_install
sudo make install
sudo update-initramfs -c -k 5.15.137

### 3. Validação (User Space)

Após iniciar no novo Kernel, compile as ferramentas de teste:
Bash

gcc estressor.c -o estressor
gcc teste_sched.c -o teste_sched

sudo update-grub
sudo reboot
tar -xf linux-5.15.137.tar.xz
cd linux-5.15.137
patch -p1 < /caminho/para/patch_projeto_final.diff

O teste de isolamento baseia-se em saturar a CPU com um processo prioritário padrão (ex: yes > /dev/null) enquanto o estressor roda na política 7. O resultado esperado e homologado é 0% de overhead sobre o processo prioritário, atestando o funcionamento perfeito do isolamento térmico da classe SCHED_BACKGROUND

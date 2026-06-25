#include "sched.h"

/* Chamada quando uma tarefa BACKGROUND acorda ou é criada.
 * Aqui você adicionará a tarefa à sua fila de processos. */
static void enqueue_task_background(struct rq *rq, struct task_struct *p, int flags)
{
    /* DECLARAÇÕES PRIMEIRO (Padrão C90) */
    struct bg_rq *bg_rq;

    /* AGORA O CÓDIGO (IF, depois atribuições) */
    if (!rq) 
        return;
    
    /* Atribuição agora é permitida após a declaração acima */
    bg_rq = &rq->bg;
    
    list_add_tail(&p->bg_run_list, &bg_rq->queue);
    bg_rq->nr_running++;
    add_nr_running(rq, 1); 
}
static void dequeue_task_background(struct rq *rq, struct task_struct *p, int flags)
{
    struct bg_rq *bg_rq = &rq->bg;

    /* Remove o gancho do processo da lista */
    list_del(&p->bg_run_list);
    
    /* Decrementa os contadores de tarefas */
    bg_rq->nr_running--;
    sub_nr_running(rq, 1);
}

/* A FUNÇÃO MAIS IMPORTANTE:
 * O Kernel chama isso quando a CPU está livre. 
 * Você só deve retornar uma tarefa se as filas do CFS e RT estiverem vazias. */
/* Escolhe o próximo processo BACKGROUND para rodar */
static struct task_struct *
pick_next_task_background(struct rq *rq)
{
    struct task_struct *p;
    struct bg_rq *bg_rq = &rq->bg;

    /* 1. Se a nossa fila estiver vazia, retornamos NULL 
     * (O Kernel passará a CPU para a idle_sched_class) */
    if (list_empty(&bg_rq->queue))
        return NULL;

    /* 2. Pega o primeiro processo que está na fila */
    p = list_first_entry(&bg_rq->queue, struct task_struct, bg_run_list);

    /* 3. Atualiza os ponteiros nativos do processador para registrar quem está rodando */
    p->se.exec_start = rq_clock_task(rq);
    
    return p;
}

/* Retira o processo atual da CPU */
static void put_prev_task_background(struct rq *rq, struct task_struct *prev)
{
    /* Em políticas simples, quando a tarefa sai da CPU, não precisamos
     * fazer grandes cálculos complexos de carga (como o CFS faz).
     * O Kernel apenas chama essa função antes de chamar o pick_next_task. */
}

static void yield_task_background(struct rq *rq)
{
}

/* Define se a tarefa atual deve ser interrompida (preemptada).
 * Como a política background é a mais baixa, qualquer outra classe pode preemptá-la. */
static void check_preempt_curr_background(struct rq *rq, struct task_struct *p, int flags)
{
    resched_curr(rq); /* Força a reavaliação do escalonamento */
}

static void set_next_task_background(struct rq *rq, struct task_struct *p, bool first)
{
    /* Registra o momento exato em que o processo assumiu a CPU */
    p->se.exec_start = rq_clock_task(rq);
}

/* Chamada a cada "tick" (pulso) do relógio do sistema.
 * Útil para implementar um Round-Robin entre as próprias tarefas background. */
static void task_tick_background(struct rq *rq, struct task_struct *p, int queued)
{
    struct bg_rq *bg_rq = &rq->bg;

    /* Se houver apenas 1 processo (ou nenhum) na fila, 
     * ele não precisa dividir a CPU com ninguém. */
    if (bg_rq->nr_running < 2)
        return;

    /* Se chegamos aqui, tem mais gente na fila!
     * A função list_move_tail tira a tarefa atual de onde ela está
     * e a realoca no final da fila (bg_rq->queue). */
    list_move_tail(&p->bg_run_list, &bg_rq->queue);

    /* O comando resched_curr() avisa o processador que ele precisa
     * parar o que está fazendo, reavaliar o escalonamento e chamar
     * o pick_next_task novamente, o que vai dar a vez ao próximo da fila. */
    resched_curr(rq);
}

/* Chamada quando o processo é acordado (ex: via kill -CONT) */
static int select_task_rq_background(struct task_struct *p, int cpu, int wake_flags)
{
    /* Retorna a CPU atual, mantendo o processo onde ele já estava */
    return task_cpu(p);
}


/* Obrigatório no 5.15: Chamado toda vez que um processo muda para BACKGROUND */
static void switched_to_background(struct rq *rq, struct task_struct *p)
{
    /* Como nossa classe é muito simples, não precisamos fazer nada,
     * mas a função DEVE existir para o Kernel não dar crash. */
}

/* Obrigatório: Chamado se a prioridade do processo for alterada */
static void prio_changed_background(struct rq *rq, struct task_struct *p, int oldprio)
{
}

/* Recomendado: O core as vezes atualiza a contabilidade de execução */
static void update_curr_background(struct rq *rq)
{
}

/* OBRIGATÓRIO PARA MÚLTIPLOS NÚCLEOS (SMP) NO KERNEL 5.15 */
#ifdef CONFIG_SMP
static struct task_struct *pick_task_background(struct rq *rq)
{
    /* Core Scheduling usa pick_task. Apenas repassamos a chamada 
     * para a sua função principal. */
    return pick_next_task_background(rq);
}
#endif

/* O macro DEFINE_SCHED_CLASS joga a estrutura na seção de memória correta do vmlinux.lds.h */
DEFINE_SCHED_CLASS(background) = {
    .enqueue_task       = enqueue_task_background,
    .dequeue_task       = dequeue_task_background,
    .yield_task         = yield_task_background,
    .check_preempt_curr = check_preempt_curr_background,

    .pick_next_task     = pick_next_task_background,
    .put_prev_task      = put_prev_task_background,

    .set_next_task      = set_next_task_background,
    .task_tick          = task_tick_background,

    /* -> NOVOS PONTEIROS ADICIONADOS AQUI <- */
    .switched_to        = switched_to_background,
    .prio_changed       = prio_changed_background,
    .update_curr        = update_curr_background,

#ifdef CONFIG_SMP
    .select_task_rq     = select_task_rq_background,
    .pick_task          = pick_task_background,
#endif
};

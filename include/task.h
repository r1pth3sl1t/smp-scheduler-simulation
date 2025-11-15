#ifndef CS_LAB1_TASK_H
#define CS_LAB1_TASK_H
#include <util.h>
#include <stdbool.h>

#define MAX_TASKS_IN_QUEUE MAX_TIME_OF_EXECUTION

struct cs_task {
    int complexity;
    int time_to_execute;
    int can_be_executed_on;
};

struct cs_task_queue {
    int size;
    int current;
    struct cs_task **tasks;
};

struct cs_task *cs_task_alloc(int complexity, int can_be_executed_on);

struct cs_task *cs_task_generate_rand(int min_complexity, int max_complexity);

int cs_tq_init(struct cs_task_queue*);
int cs_tq_destroy(struct cs_task_queue*);
int cs_tq_add_task(struct cs_task_queue *, struct cs_task*);
struct cs_task *cs_tq_next(struct cs_task_queue*);

struct cs_task *cs_tq_cur(struct cs_task_queue *);

static inline bool cs_task_can_be_executed_on(struct cs_task *task, int processor_id)
{
    return (task->can_be_executed_on & (1 << processor_id)) != 0;
}


#endif //CS_LAB1_TASK_H

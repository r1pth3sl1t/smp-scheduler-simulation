#include <task.h>
#include <stdlib.h>
#include <memory.h>

struct cs_task *cs_task_alloc(int complexity, int can_be_executed_on)
{

    struct cs_task *task = (struct cs_task *)malloc(sizeof (struct cs_task));
    if (!task)
        return task;

    task->complexity = complexity;
    task->time_to_execute = 0;
    task->can_be_executed_on = can_be_executed_on;
    cs_debug("task created, %d %d %x", task->complexity, task->time_to_execute, task->can_be_executed_on);

    return task;
}

int cs_tq_init(struct cs_task_queue* queue)
{
    if (!queue)
        return EINVAL;

    queue->size = 0;
    queue->current = 0;
    queue->tasks = malloc(sizeof (struct cs_task*) * MAX_TASKS_IN_QUEUE);
    if (!queue->tasks)
        return ENOMEM;
    memset(queue->tasks, 0, MAX_TASKS_IN_QUEUE * sizeof (struct cs_task*));
    return 0;
}

int cs_tq_destroy(struct cs_task_queue* queue)
{
    if (queue->tasks)
        free(queue->tasks);

    return 0;
}

int cs_tq_add_task(struct cs_task_queue *queue, struct cs_task *task)
{
    if(queue->size == MAX_TASKS_IN_QUEUE)
        return ENOMEM;

    queue->tasks[queue->size++] = task;
    return 0;
}

struct cs_task *cs_tq_next(struct cs_task_queue* queue)
{
    if (queue->current >= queue->size)
        return NULL;
    return queue->tasks[queue->current++];
}

struct cs_task *cs_task_generate_rand(int min_complexity, int max_complexity)
{

    return cs_task_alloc((rand() % (200 * max_complexity - 50 * min_complexity + 1)) + 50 * min_complexity,
                         (1 << (rand() & (MAX_PROCESSORS_NUM))) | (rand() % (1 << MAX_PROCESSORS_NUM)));
}

struct cs_task *cs_tq_cur(struct cs_task_queue *queue)
{
    if (queue->current >= queue->size)
        return NULL;
    return queue->tasks[queue->current];
}
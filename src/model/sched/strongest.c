#include <malloc.h>
#include "scheduler.h"

#define SCHEDULER_PROCESSOR_EXECUTION_TIME 20
#define SCHEDULER_PROCESSOR_SCHEDULING_TIME 4

struct cs_processor_scheduler_priv {
    struct cs_task *task;
    int scheduling_time_left;
    int execution_time_left;
};

static inline struct cs_processor* get_strongest_processor(struct cs_scheduler *scheduler)
{
    int max_idx = 0;
    for(int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (scheduler->processors[i]->computing_power > scheduler->processors[max_idx]->computing_power)
            max_idx = i;
    }

    return scheduler->processors[max_idx];
}

static void cs_strongest_processor_scheduler_schedule(struct cs_scheduler *scheduler, struct cs_task *task)
{
    bool task_assigned = false, assign_task_from_queue = false;
    struct cs_task *task_to_assign = NULL;
    struct cs_processor *strongest_processor = get_strongest_processor(scheduler);
    int fastest_processor_idx = 0;
    struct cs_processor_scheduler_priv *priv = NULL;

    if (!strongest_processor->priv) {
        strongest_processor->priv = malloc(sizeof(struct cs_processor_scheduler_priv));
        if (!strongest_processor->priv)
            return;

        priv = strongest_processor->priv;
        priv->scheduling_time_left = SCHEDULER_PROCESSOR_SCHEDULING_TIME;
        priv->execution_time_left  = SCHEDULER_PROCESSOR_EXECUTION_TIME;
    }
    priv = strongest_processor->priv;

    if (scheduler->queue.size > 0)
        task_to_assign = cs_tq_cur(&scheduler->queue);

    if (!task_to_assign || (task && task_to_assign->complexity > task->complexity)) {
        task_to_assign = task;
    }
    else
        assign_task_from_queue = true;

    if (!task_to_assign) {
        cs_debug("nothing to schedule");
        return;
    }

    fastest_processor_idx = 0;
    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (scheduler->processors[i] == strongest_processor) {
            if (!strongest_processor->busy && !priv->scheduling_time_left) {
                fastest_processor_idx = i;
                break;
            }

            if (priv->execution_time_left) {
                priv->execution_time_left--;
                continue;
            }
            else if (priv->scheduling_time_left) {
                strongest_processor->busy = false;
                priv->scheduling_time_left--;
                continue;
            }
            else {
                priv->scheduling_time_left = SCHEDULER_PROCESSOR_SCHEDULING_TIME;
                priv->execution_time_left = SCHEDULER_PROCESSOR_EXECUTION_TIME;
            }
        }

        if (scheduler->processors[i]->busy) {
            continue;
        }
        if (!cs_task_can_be_executed_on(task_to_assign, scheduler->processors[i]->id)) {
            continue;
        }
        cs_debug("task %i %i task2 %i %i", i, cs_processor_get_execution_time(scheduler->processors[i], task_to_assign),
                 fastest_processor_idx, cs_processor_get_execution_time(scheduler->processors[fastest_processor_idx], task_to_assign));
        if (cs_processor_get_execution_time(scheduler->processors[i], task_to_assign) <
            cs_processor_get_execution_time(scheduler->processors[fastest_processor_idx], task_to_assign) ||
            scheduler->processors[fastest_processor_idx]->busy) {
            fastest_processor_idx = i;
        }
    }

    if (!scheduler->processors[fastest_processor_idx]->busy)
    {
        cs_processor_add_task(scheduler->processors[fastest_processor_idx], task_to_assign);
        task_assigned = true;
        cs_debug("Assigned to %d", fastest_processor_idx);
    }

    if (!task_assigned && task)
        cs_tq_add_task(&scheduler->queue, task);
    else if (scheduler->queue.current < scheduler->queue.size &&
             assign_task_from_queue &&
             task_assigned)
        cs_tq_next(&scheduler->queue);
}

static const char *cs_strongest_processor_scheduler_name(struct cs_scheduler *unused)
{
    return "Strongest processor";
}

const struct cs_scheduler_ops cs_strongest_processor_scheduler_ops = {
        .schedule = cs_strongest_processor_scheduler_schedule,
        .name     = cs_strongest_processor_scheduler_name
};
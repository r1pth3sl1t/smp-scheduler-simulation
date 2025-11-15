#include "scheduler.h"

static void cs_fifo_scheduler_schedule(struct cs_scheduler *scheduler, struct cs_task *task)
{
    bool task_assigned = false, assign_task_from_queue = false;
    struct cs_task *task_to_assign = NULL;
    if (scheduler->queue.size > 0)
        task_to_assign = cs_tq_cur(&scheduler->queue);

    if (!task_to_assign) {
        task_to_assign = task;
    }
    else
        assign_task_from_queue = true;

    if (!task_to_assign) {
        cs_debug("nothing to schedule");
        return;
    }

    for(int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (!scheduler->processors[i])
            continue;

        if (scheduler->processors[i]->busy)
            continue;

        cs_debug("%d %d %d", cs_task_can_be_executed_on(task_to_assign, scheduler->processors[i]->id),
                 scheduler->processors[i]->id,
                 task_to_assign->can_be_executed_on);
        if (cs_task_can_be_executed_on(task_to_assign, scheduler->processors[i]->id))
        {
            cs_processor_add_task(scheduler->processors[i], task_to_assign);
            task_assigned = true;
            cs_debug("Assigned to %d", i);
            break;
        }
    }

    if (!task_assigned && task)
        cs_tq_add_task(&scheduler->queue, task);
    else if (scheduler->queue.current < scheduler->queue.size &&
            assign_task_from_queue &&
            task_assigned)
        cs_tq_next(&scheduler->queue);
}

static const char *cs_fifo_scheduler_name(struct cs_scheduler *unused)
{
    return "FIFO";
}

const struct cs_scheduler_ops cs_fifo_scheduler_ops = {
        .schedule = cs_fifo_scheduler_schedule,
        .name     = cs_fifo_scheduler_name,
};
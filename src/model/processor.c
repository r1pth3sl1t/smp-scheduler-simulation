#include <processor.h>
#include <errno.h>
#include <malloc.h>

void cs_processor_init(struct cs_processor *processor)
{
    processor->stats.time_slept = 0;
    processor->stats.time_worked = 0;
    processor->busy = false;
    processor->pending_task = NULL;
    processor->priv = NULL;
}

static inline void cs_processor_sleep(struct cs_processor *processor)
{
    processor->stats.time_slept++;
    cs_debug("processor %d sleeping", processor->id);
}

int cs_processor_get_execution_time(struct cs_processor* processor, struct cs_task* task)
{
    if (!processor || !task)
        return INT_MAX;

    return task->complexity / processor->computing_power;
}

int cs_processor_add_task(struct cs_processor *processor, struct cs_task *task)
{
    if (!processor || !task)
        return EINVAL;

    if (processor->busy)
        return EBUSY;

    processor->pending_task = task;
    processor->busy = true;

    task->time_to_execute = task->complexity / processor->computing_power;
    if (!task->time_to_execute)
        task->time_to_execute = 1;
    cs_debug("added to processor %d time to execute %d", processor->id, task->time_to_execute);
    return 0;
}

static void cs_processor_work(struct cs_processor *processor)
{
    struct cs_task *task = processor->pending_task;
    if (!task)
        return;

    processor->stats.time_worked++;
    task->time_to_execute--;
    cs_debug("processor %d working, time left %d", processor->id, processor->pending_task->time_to_execute);
    if (!task->time_to_execute) {
        processor->busy = false;
        free(task);
        processor->pending_task = NULL;
        return;
    }
}

void cs_processor_run(struct cs_processor *processor)
{
    if (processor->busy)
        cs_processor_work(processor);
    else
        cs_processor_sleep(processor);
}

void cs_processor_dump_stats(struct cs_processor *processor)
{
    cs_info("Processor [%d] worked %5d slept %5d - %.2lf%% of effective usage", processor->id,
             processor->stats.time_worked,
             processor->stats.time_slept,
             (double)processor->stats.time_worked / (processor->stats.time_slept + processor->stats.time_worked) * 100);
}
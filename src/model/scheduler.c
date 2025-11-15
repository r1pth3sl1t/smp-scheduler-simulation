#include <scheduler.h>
#include <stdlib.h>

extern const struct cs_scheduler_ops cs_fifo_scheduler_ops;
extern const struct cs_scheduler_ops cs_weakest_processor_scheduler_ops;
extern const struct cs_scheduler_ops cs_strongest_processor_scheduler_ops;

static inline int get_min_computing_power(struct cs_scheduler *scheduler)
{
    int min_pwr = INT_MAX;
    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (!scheduler->processors[i]) continue;

        min_pwr = __min(min_pwr, scheduler->processors[i]->computing_power);
    }
    return min_pwr;
}

static inline int get_max_computing_power(struct cs_scheduler *scheduler)
{
    int max_pwr = INT_MIN;
    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (!scheduler->processors[i]) continue;

        max_pwr = __max(max_pwr, scheduler->processors[i]->computing_power);
    }
    return max_pwr;
}

static void cs_scheduler_show_running_stats(struct cs_scheduler *scheduler)
{
    int max_operations = 0, overall_operations = 0;
    for (int i = 0; i < MAX_PROCESSORS_NUM; i++) {
        max_operations += scheduler->processors[i]->computing_power * MAX_TIME_OF_EXECUTION;
        overall_operations += scheduler->processors[i]->computing_power * scheduler->processors[i]->stats.time_worked;
    }

    if (scheduler->ops->name)
        cs_info("%s", scheduler->ops->name(scheduler));

    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
        cs_processor_dump_stats(scheduler->processors[i]);

    cs_info("General system productivity: %.2lf%%", ((double)overall_operations / max_operations) * 100.0);
}

int cs_scheduler_reset(struct cs_scheduler *scheduler)
{
    scheduler->time_left = MAX_TIME_OF_EXECUTION;
    for (int i = scheduler->queue.current; i < scheduler->queue.size; i++)
    {
        if (cs_tq_cur(&scheduler->queue))
            free(cs_tq_cur(&scheduler->queue));
        cs_tq_next(&scheduler->queue);
    }
    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (scheduler->processors[i]->priv)
            free(scheduler->processors[i]->priv);

        cs_processor_init(scheduler->processors[i]);
    }
    scheduler->queue.current = 0;
    scheduler->queue.size = 0;
    return 0;
}

int cs_scheduler_run(struct cs_scheduler *scheduler)
{
    struct cs_task *task = NULL;
    int min_complexity, max_complexity;

    if (!scheduler || !scheduler->ops) {
        cs_debug("scheduler is not initialized");
        return EINVAL;
    }
    min_complexity = get_min_computing_power(scheduler);
    max_complexity = get_max_computing_power(scheduler);

    for(;scheduler->time_left; scheduler->time_left--)
    {
         if (rand() % 100 < scheduler->config->task_creation_probability)
        {
            task = cs_task_generate_rand(min_complexity, max_complexity, scheduler->config);
            if (!task)
            {
                cs_debug("task allocation failed");
                return ENOMEM;
            }
        }

        scheduler->ops->schedule(scheduler, task);
        task = NULL;
        for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
            cs_processor_run(scheduler->processors[i]);
    }

    cs_scheduler_show_running_stats(scheduler);
    return 0;
}

int cs_scheduler_init(struct cs_scheduler *scheduler)
{
    int ret = 0;
    if (scheduler == NULL) {
        cs_debug("scheduler is NULL");
        return EINVAL;
    }

    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        scheduler->processors[i] = malloc(sizeof (struct cs_processor));
        if (!scheduler->processors[i]) {
            cs_debug("Failed to allocate processor %d", i);
            return ENOMEM;
        }
        cs_processor_init(scheduler->processors[i]);
        scheduler->processors[i]->id = i;
        scheduler->processors[i]->busy = false;
        scheduler->processors[i]->computing_power = global_cs_processor_config[i].computing_power;
        scheduler->processors[i]->id = i;
    }

    ret = cs_tq_init(&scheduler->queue);
    if (ret)
    {
        cs_debug("scheduler task queue allocation failed");
        if (scheduler->config)
            free(scheduler->config);
        for (int j = 0; j < MAX_PROCESSORS_NUM; j++)
            if (scheduler->processors[j])
                free(scheduler->processors[j]);
        return ret;
    }

    scheduler->time_left = MAX_TIME_OF_EXECUTION;
    return 0;
}

int cs_scheduler_ops_init(struct cs_scheduler *scheduler, const struct cs_scheduler_ops *ops)
{
    if (!scheduler || !ops) {
        cs_debug("scheduler or ops is NULL");
        return EINVAL;
    }

    scheduler->ops = ops;
    return 0;
}

int cs_scheduler_destroy(struct cs_scheduler *scheduler)
{
    int ret = 0;
    for (int i = 0; i < MAX_PROCESSORS_NUM; i++)
    {
        if (scheduler->processors[i]) {
            if (scheduler->processors[i]->priv)
                free(scheduler->processors[i]->priv);

            free(scheduler->processors[i]);
            scheduler->processors[i] = NULL;
        }
    }
    if (scheduler->config)
        free(scheduler->config);
    ret = cs_tq_destroy(&scheduler->queue);
    if (ret)
        return ret;

    return 0;
}

const struct cs_scheduler_ops *strategy_to_ops_map[] = {
        [CS_SCHED_FIFO] = &cs_fifo_scheduler_ops,
        [CS_SCHED_WEAKEST] = &cs_weakest_processor_scheduler_ops,
        [CS_SCHED_STRONGEST] = &cs_strongest_processor_scheduler_ops
};
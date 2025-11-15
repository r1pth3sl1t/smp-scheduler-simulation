#ifndef CS_LAB1_SCHEDULER_H
#define CS_LAB1_SCHEDULER_H
#include <util.h>
#include <processor.h>
#include <config.h>

struct cs_scheduler {
    int time_left;
    struct cs_task_queue queue;
    struct cs_processor *processors[MAX_PROCESSORS_NUM];
    const struct cs_scheduler_ops *ops;
    struct global_simulation_config *config;
};

struct cs_scheduler_ops {
    void (*schedule)(struct cs_scheduler *, struct cs_task *);
    const char* (*name)(struct cs_scheduler*);
};

enum cs_scheduler_strategy {
    CS_SCHED_FIFO = 0,
    CS_SCHED_WEAKEST,
    CS_SCHED_STRONGEST,

    CS_SCHED_STRATEGY_SIZE
};

extern const struct cs_scheduler_ops *strategy_to_ops_map[];

int cs_scheduler_init(struct cs_scheduler *);
int cs_scheduler_ops_init(struct cs_scheduler *, const struct cs_scheduler_ops *);
int cs_scheduler_destroy(struct cs_scheduler *);
int cs_scheduler_run(struct cs_scheduler *);
int cs_scheduler_reset(struct cs_scheduler *);

static inline int run_for_strategy(struct cs_scheduler * scheduler, enum cs_scheduler_strategy strategy)
{
    int ret = cs_scheduler_ops_init(scheduler, strategy_to_ops_map[strategy]);
    if (ret)
        return ret;

    cs_scheduler_run(scheduler);
    cs_scheduler_reset(scheduler);

    return 0;
}

#endif //CS_LAB1_SCHEDULER_H

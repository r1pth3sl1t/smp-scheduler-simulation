#include <stdio.h>
#include <stdlib.h>
#include <scheduler.h>
#include <time.h>

int main() {
    struct cs_scheduler *sched;
    struct global_simulation_config *config;
    int ret;

    srand(time(NULL));

    sched = malloc(sizeof (struct cs_scheduler));
    if (!sched)
        return ENOMEM;

    ret = cs_scheduler_init(sched);
    if (ret)
        return ret;

    config = create_global_config();
    if (!config)
        return EINVAL;

    sched->config = config;

    ret = cs_scheduler_ops_init(sched, strategy_to_ops_map[CS_SCHED_WEAKEST]);
    if (ret)
        return ret;

    for (int i = 0; i < CS_SCHED_STRATEGY_SIZE; i++)
    {
        ret = run_for_strategy(sched, i);
        if (ret)
            return ret;
    }

    ret = cs_scheduler_destroy(sched);
    if (ret)
        return ret;
    return 0;
}

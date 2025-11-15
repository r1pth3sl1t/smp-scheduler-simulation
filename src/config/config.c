#include "config.h"
#include <stdio.h>
#include <malloc.h>
#include <stdbool.h>

const struct global_processor_config global_cs_processor_config[MAX_PROCESSORS_NUM] = {
        {0, 5},
        {1, 4},
        {2, 7},
        {3, 2},
        {4, 10},
        {5, 1},
        {6, 21},
        {7, 7},
        {8, 16},
        {9, 10},
};

static inline int type_config(int *field, const char *prompt)
{
    int val, ret;
    cs_info("%s", prompt);
    printf(">   ");
    ret = scanf("%d", &val);
    if (ret != 1)
        return EINVAL;

    *field = val;
    return 0;
}

static bool is_config_valid(struct global_simulation_config *config)
{
    if (config->min_time_of_execution >= config->max_time_of_execution)
        return false;

    if (config->min_time_of_execution <= 0 || config->max_time_of_execution <= 0)
        return false;

    if (config->task_creation_probability < 0)
        return false;

    return true;
}

struct global_simulation_config *create_global_config()
{
    int ret;
    struct global_simulation_config *config = malloc(sizeof (struct global_simulation_config));
    if (!config)
        return NULL;

    ret = type_config(&config->task_creation_probability, "Type task creation probability (%%, per tick)");
    if (ret)
        goto fail;
    ret = type_config(&config->min_time_of_execution, "Type min task execution time (on weakest processor ,ms)");
    if (ret)
        goto fail;
    ret = type_config(&config->max_time_of_execution, "Type max task execution time (on weakest processor ,ms)");
    if (ret)
        goto fail;
    if (config->min_time_of_execution > config->max_time_of_execution)
        goto fail;
    if (!is_config_valid(config))
        goto fail;

    return config;
fail:
    cs_info("config creation falied");
    free(config);
    return NULL;
}
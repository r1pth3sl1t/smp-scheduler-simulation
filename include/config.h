#ifndef CS_LAB1_CONFIG_H
#define CS_LAB1_CONFIG_H
#include <util.h>

struct global_processor_config {
    int id;
    int computing_power;
};

struct global_simulation_config {
    int task_creation_probability;
    int min_time_of_execution;
    int max_time_of_execution;
};

struct global_simulation_config *create_global_config();

extern const struct global_processor_config global_cs_processor_config[MAX_PROCESSORS_NUM];


#endif //CS_LAB1_CONFIG_H

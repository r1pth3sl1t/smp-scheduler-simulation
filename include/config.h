#ifndef CS_LAB1_CONFIG_H
#define CS_LAB1_CONFIG_H
#include <util.h>

struct global_processor_config {
    int id;
    int computing_power;
};

extern const struct global_processor_config global_cs_processor_config[MAX_PROCESSORS_NUM];

#endif //CS_LAB1_CONFIG_H

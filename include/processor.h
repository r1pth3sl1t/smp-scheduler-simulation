#ifndef CS_LAB1_PROCESSOR_H
#define CS_LAB1_PROCESSOR_H
#include <stdbool.h>
#include <task.h>

struct cs_processor_stats {
    int time_worked;
    int time_slept;
};

struct cs_processor {
    int id;
    int computing_power;
    bool busy;
    struct cs_task *pending_task;
    struct cs_processor_stats stats;
    void *priv;
};

void cs_processor_init(struct cs_processor *);
void cs_processor_run(struct cs_processor *);
int cs_processor_add_task(struct cs_processor *, struct cs_task *);
void cs_processor_dump_stats(struct cs_processor *);
int cs_processor_get_execution_time(struct cs_processor*, struct cs_task*);



#endif //CS_LAB1_PROCESSOR_H

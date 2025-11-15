#ifndef CS_LAB1_UTIL_H
#define CS_LAB1_UTIL_H
#include <stdio.h>

#define DEBUG 0
#define MAX_TIME_OF_EXECUTION 10000
#define MAX_PROCESSORS_NUM 10

#define cs_info(fmt, ...) do { printf("[INFO] " fmt "\n", ##__VA_ARGS__); } while (0)

#if DEBUG
#define cs_debug(fmt, ...) do { fprintf(stdout, "[DEBUG %s] " fmt "\n", __func__, ##__VA_ARGS__); } while (0)
#else
#define cs_debug(fmt, ...) ;
#endif

#endif //CS_LAB1_UTIL_H

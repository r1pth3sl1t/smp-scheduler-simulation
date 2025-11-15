cmake_minimum_required(VERSION 3.27)
project(cs_lab1 C)

set(CMAKE_C_STANDARD 11)

add_executable(cs_lab1 src/main.c
        src/model/processor.c
        include/processor.h
        include/scheduler.h
        include/task.h
        include/util.h
        include/util.h
        src/model/task.c
        src/model/scheduler.c
        include/config.h
        src/model/sched/fifo.c
        src/config/config.c
        src/model/sched/weakest.c
        src/model/sched/strongest.c)

target_include_directories(cs_lab1 PRIVATE include)
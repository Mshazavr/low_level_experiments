#pragma once 

#include <sys/time.h>

long get_microseconds();

template<typename T, typename... Args>
float time_function(T f, Args... args);

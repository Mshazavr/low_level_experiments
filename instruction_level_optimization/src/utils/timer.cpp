#include <sys/time.h>
#include <chrono>
#include "timer.hpp"

long get_microseconds(){
	timeval current_time;
	gettimeofday(&current_time, 0);
	return current_time.tv_sec * (int)1e6 + current_time.tv_usec;
}

template<typename T, typename... Args>
float time_function(T f, Args... args) {
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    f(args...);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
}

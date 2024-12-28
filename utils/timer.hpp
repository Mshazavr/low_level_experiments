#pragma once 

#include <sys/time.h>

long get_microseconds(){
	timeval current_time;
	gettimeofday(&current_time, 0);
	return current_time.tv_sec * (int)1e6 + current_time.tv_usec;
}
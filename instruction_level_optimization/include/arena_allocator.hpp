#pragma once

#include <cstdlib>

/*
Simple arena allocator.

Useful for programs that 
1. allocate a bunch of memory in the beginning
2. play with the memory 
3. free all the memory towards the end

In other words, the lifetime of the allocated objects is 
the entire program.

Using the arena allocator will avoid the unnecessary 
overhead that comes with the lifetime spagetti with 
multi-purpose malloc and free and RAII
*/
struct Arena {
    size_t capacity; 
    size_t next_free; 
    void *buffer;

    size_t alignment;
};

Arena arena_create(size_t capacity, size_t alignment);

void *arena_alloc(Arena &arena, size_t num_bytes);

void *arena_alloc_aligned(Arena &arena, size_t num_bytes, size_t alignment);

void arena_reset(Arena &arena);

void arena_destroy(Arena &arena);

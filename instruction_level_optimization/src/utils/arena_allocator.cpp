#include <cstdlib>
#include <iostream>

#include "arena_allocator.hpp"

Arena arena_create(size_t capacity, size_t alignment) {
    void *buffer = std::aligned_alloc(alignment, capacity);
    return {
        .capacity = capacity,
        .next_free = 0,
        .buffer = buffer,
        .alignment = alignment
    };
}

void *arena_alloc(Arena &arena, size_t num_bytes) {
    if (arena.next_free + num_bytes > arena.capacity) {
        std::cerr << "Arena overflow. Exitting..." << std::endl;
        exit(1);
    }
    void *result = (char *)arena.buffer + arena.next_free;
    arena.next_free += num_bytes;
    return result;
}

void *arena_alloc_aligned(Arena &arena, size_t num_bytes, size_t alignment) {
    if (((size_t)arena.buffer + arena.next_free) % alignment != 0) {
        arena.next_free += alignment - (((size_t)arena.buffer + arena.next_free) % arena.alignment);
    }
    return arena_alloc(arena, num_bytes);
}

void arena_reset(Arena &arena) {
    arena.next_free = 0;
}

void arena_destroy(Arena &arena) {
    free(arena.buffer);
}
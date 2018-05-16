#pragma once

#include <lib/types.h>

typedef uint32_t mutex_t;

void acquire_spinlock(mutex_t *lock);
void free_spinlock(mutex_t *lock);
void init_spinlock(mutex_t *lock);

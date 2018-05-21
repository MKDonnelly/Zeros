#pragma once

#include <lib/types.h>

typedef uint32_t mutex_t;

void spinlock_acquire(mutex_t *lock);
void spinlock_release(mutex_t *lock);
void spinlock_init(mutex_t *lock);

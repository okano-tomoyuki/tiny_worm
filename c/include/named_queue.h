#ifndef NAMED_QUEUE_H
#define NAMED_QUEUE_H

#include <stdint.h>

struct named_queue_impl_t;
typedef struct named_queue_t
{
    struct named_queue_impl_t* impl;
} named_queue_t;

int named_queue_create(const char* name, size_t type_size, size_t buff_size);

named_queue_t* named_queue_attach(const char* name);

void named_queue_detach(named_queue_t* nq);

int named_queue_empty(named_queue_t* nq);

void named_queue_clear(named_queue_t* nq);

size_t named_queue_size(named_queue_t* nq);

int named_queue_full(named_queue_t* nq);

void named_queue_push(named_queue_t* nq, const void* value);

void named_queue_pop(named_queue_t* nq);

void named_queue_front(named_queue_t* nq, void* ret);

void named_queue_back(named_queue_t* nq, void* ret);

#endif // NAMED_QUEUE_H
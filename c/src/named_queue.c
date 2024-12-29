#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "named_queue.h"

typedef struct named_queue_impl_t
{
    char*   name;
    size_t  count;
    size_t  size;
    size_t  head;
    size_t  tail;
    char*   data;
    struct named_queue_impl_t*  next;
} named_queue_impl_t;

static named_queue_impl_t* table_entry = NULL;

static named_queue_impl_t* named_queue_get_table(void)
{
    if (table_entry == NULL)
    {
        table_entry = (named_queue_impl_t*)malloc(sizeof(named_queue_impl_t));
        table_entry->next = NULL;
        table_entry->name = NULL;
        table_entry->data = NULL;
    }
    return table_entry;
};

int named_queue_create(const char* name, size_t type_size, size_t buff_size)
{
    named_queue_impl_t* entry   = named_queue_get_table();
    while (entry->next)
    {
        entry = entry->next;
        if (strcmp(name, entry->name) == 0)
            return 0;
    }

    named_queue_impl_t* nq_impl = (named_queue_impl_t*)malloc(sizeof(named_queue_impl_t));
    nq_impl->name           = (char*)malloc(strlen(name) + 1);
    strcpy(nq_impl->name, name);
    nq_impl->count          = 0;
    nq_impl->size           = buff_size;
    nq_impl->head           = 0;
    nq_impl->tail           = 0;
    nq_impl->data           = (char*)malloc(type_size * buff_size);
    nq_impl->next           = NULL;
    entry->next             = nq_impl;

    if (nq_impl->data == NULL)
    {
        printf("malloc.");
    }

    return 1;
}

named_queue_t* named_queue_attach(const char* name)
{
    named_queue_impl_t* entry   = named_queue_get_table();
    while (entry->next)
    {
        entry = entry->next;
        if (strcmp(name, entry->name) == 0)
        {
            named_queue_t* nq  = (named_queue_t*)malloc(sizeof(named_queue_t));
            nq->impl = entry;
            nq->impl->count++;
            return nq;
        }
    }
    return NULL;
}

void named_queue_detach(named_queue_t* nq)
{
    nq->impl->count--;
    if (nq->impl->count == 0)
    {
        named_queue_impl_t* entry   = named_queue_get_table();
        while (entry->next)
        {
            if (strcmp(nq->impl->name, entry->next->name) == 0)
            {
                free(nq->impl->name);
                free(nq->impl->data);
                free(nq->impl);

                nq->impl->name  = NULL;
                nq->impl->data  = NULL;
                nq->impl        = NULL;

                entry->next     = entry->next->next;
                return;
            }
            entry = entry->next;
        }
    }
}

int named_queue_empty(named_queue_t* nq)
{
    return (nq->impl->head == nq->impl->tail);
}

void named_queue_clear(named_queue_t* nq)
{
    nq->impl->head = 0;
    nq->impl->tail = 0;
}

size_t named_queue_size(named_queue_t* nq)
{
    return (nq->impl->size + nq->impl->tail - nq->impl->head) % nq->impl->size;
}

int named_queue_full(named_queue_t* nq)
{
    return (nq->impl->head == ((nq->impl->tail + 1) % nq->impl->size));
}

void named_queue_push(named_queue_t* nq, const void* value)
{
    memcpy(nq->impl->data + (nq->impl->tail * nq->impl->size), value, nq->impl->size);
    nq->impl->tail = (nq->impl->tail + 1) % nq->impl->size;
}

void named_queue_pop(named_queue_t* nq)
{
    nq->impl->head = (nq->impl->head + 1) % nq->impl->size;
}

void named_queue_front(named_queue_t* nq, void* ret)
{
    memcpy(ret, nq->impl->data + nq->impl->head * nq->impl->size, nq->impl->size);
}

void named_queue_back(named_queue_t* nq, void* ret)
{
    memcpy(ret, nq->impl->data + ((nq->impl->size + nq->impl->tail - 1) % nq->impl->size)  * nq->impl->size, nq->impl->size);
}
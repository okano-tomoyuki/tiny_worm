#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "named_queue.h"

const char NQ_NAME1[16] = "NQ_NAME1";
const char NQ_NAME2[16] = "NQ_NAME2";
const int  MAX_ITER     = 20;

typedef struct Person
{
    int   counter;
    char name[16];
    double height;
    double weight;
} Person;

void publish_func(named_queue_t* nq);
void subscribe_func(named_queue_t* nq);

int main(void)
{
    named_queue_create(NQ_NAME1, sizeof(Person), 50);

    named_queue_t* nq_pub = named_queue_attach(NQ_NAME1);
    named_queue_t* nq_sub = named_queue_attach(NQ_NAME1);

    publish_func(nq_pub);
    subscribe_func(nq_sub);

    named_queue_detach(nq_pub);
    named_queue_detach(nq_sub);
}

void publish_func(named_queue_t* nq)
{
    Person person;
    for (int i = 0; i < MAX_ITER; i++)
    {
        sprintf(person.name, "person-%03d", i + 1);
        person.height = 160.0 + i * 0.1;
        person.weight =  60.0 + i * 0.2;
        named_queue_push(nq, &person);
    }
}

void subscribe_func(named_queue_t* nq)
{
    Person person;
    while (!named_queue_empty(nq))
    {
        printf("size : %u ", named_queue_size(nq));
        named_queue_front(nq, &person);
        printf("name : %s, height : %f, weight : %f\n", person.name, person.height, person.weight);
        named_queue_pop(nq);
    }
}
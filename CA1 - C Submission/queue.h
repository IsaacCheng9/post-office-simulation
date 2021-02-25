/* Header file for handling the queue, which is implemented using linked
 * lists. */
#ifndef __QUEUE_H
#define __QUEUE_H

#include <errno.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <customer.h>

/* Queue structure in a linked list, with front and rear pointers. */
struct queue
{
    struct customer *front, *rear;
    int queue_length, max_queue_length;
};
typedef struct queue QUEUE;

/* Queue function prototypes. */
QUEUE *create_empty_queue(int);
int is_queue_empty(QUEUE *);
void increment_waiting_times(QUEUE *);
void enqueue(QUEUE *, int, int, int, int, gsl_rng *);
int dequeue(QUEUE *);
int fulfil_customer(QUEUE *, int, int *, int);
int serve_customers(int, int, int *);
int leave_queue_early(QUEUE *, int);
int is_branch_empty(QUEUE *, int, int *);

#endif


/* Queue implementation as a linked list. */
#include <stdio.h>
#include <stdlib.h>

/* Node structure using a linked list. */
struct node
{
    int data;
    struct node *next;
};
typedef struct node NODE;

/* Queue structure in a linked list, with front and rear pointers. */
struct queue
{
    struct node *front, *rear;
};
typedef struct queue QUEUE;

/* Queue function prototypes */
NODE *new_node(int);
QUEUE *initialise();
int is_empty(QUEUE *);
void enqueue(QUEUE *, int);
void dequeue(QUEUE *);
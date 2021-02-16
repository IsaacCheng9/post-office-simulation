/* Queue implementation as a linked list. */
#include <stdio.h>
#include <stdlib.h>

/* Node structure using a linked list. */
struct node
{
    int key;
    struct node *next;
};
typedef struct node NODE;

/* Queue structure in a linked list, with front and rear pointers. */
struct queue
{
    struct node *front, *rear;
};
typedef struct queue QUEUE;

/* Creates a new linked list node to represent a person. */
struct node *new_node(int value)
{
    struct node *person = (struct node *)malloc(sizeof(struct node));
    person->key = value;
    person->next = NULL;
    return person;
}

/* Creates an empty queue. */
struct queue *initialise()
{
    struct queue *q = (struct queue *)malloc(sizeof(struct queue));
    q->front = q->rear = NULL;
    return q;
}

/* Function prototypes */
int is_empty(QUEUE *);
void enqueue(QUEUE *, int);
void dequeue(QUEUE *);

/* Other functions */

int is_empty(QUEUE *q)
{
    return (q->rear == NULL);
}

/* Adds a value onto the end of the queue and increases queue count. */
void enqueue(QUEUE *q, int value)
{
    // Creates a new node and its data and next node pointer.
    NODE *person = new_node(value);

    /* Points front and rear of the queue to the new node if empty. Otherwise,
    points the previous rear of the node to this node, and sets the new node as
    the rear. */
    if (is_empty(q))
    {
        q->front = q->rear = person;
        return;
    }
    else
    {
        q->rear->next = person;
        q->rear = person;
    }
}

/* Removes a value from the front of the queue. */
void dequeue(QUEUE *q)
{
    // Returns NULL if the queue is empty.
    if (is_empty(q))
    {
        return;
    }

    // Stores the previous front of the queue, and moves the next person up.
    NODE *person = q->front;
    q->front = q->front->next;

    /* If this makes the front empty, make the rear empty, as this means the
    queue is empty. */
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(person);
}

/* Main function */
int main()
{
    QUEUE *q = initialise();
    enqueue(q, 10);
    enqueue(q, 20);
    dequeue(q);
    dequeue(q);
    enqueue(q, 30);
    enqueue(q, 40);
    enqueue(q, 50);
    dequeue(q);
    printf("Queue Front: %d\n", q->front->key);
    printf("Queue Rear: %d\n", q->rear->key);

    return 0;
}
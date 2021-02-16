/* Queue implementation as a linked list. */
#include <queue.h>

/* Creates a new linked list node to represent a person. */
NODE *new_node(int value)
{
    NODE *person = (NODE *)malloc(sizeof(NODE));
    person->data = value;
    person->next = NULL;
    return person;
}

/* Creates an empty queue. */
QUEUE *initialise()
{
    QUEUE *q = (QUEUE *)malloc(sizeof(QUEUE));
    q->front = q->rear = NULL;
    return q;
}

/* Checks if the queue is empty. */
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
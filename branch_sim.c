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

/* Function prototypes */
int *create_service_points(int);
NODE *new_node(int);
QUEUE *initialise();
int is_empty(QUEUE *);
void enqueue(QUEUE *, int);
void dequeue(QUEUE *);


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
    printf("Queue Front: %d\n", q->front->data);
    printf("Queue Rear: %d\n", q->rear->data);

    // Creates a random number of service points.
    int points = rand();
    int *service_points = (int *)create_service_points(points);

    printf("Number of Service Points: %d\n", points);
    int j;
    for (j = 0; j < points; j++)
    {
        printf("Service Point: %d, Value: %d\n", j, service_points[j]);
    }

    return 0;
}

/* Other functions */

/* Creates the empty service points to start with. */
int *create_service_points(int points)
{
    int i;
    // Dynamically allocates memory according to the number of service points.
    int *service_points = NULL;
    if (!(service_points = (int *)malloc(points * sizeof(int))))
    {
        printf("Out of memory.\n");
        exit(1);
    };

    // Assigns each service point to represent them being empty.
    for (i = 0; i < points; i++)
    {
        service_points[i] = 0;
    }

    return service_points;
}

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

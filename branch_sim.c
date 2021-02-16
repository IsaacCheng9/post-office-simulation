/* Queue implementation as a linked list. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    struct node *front, *rear, *size, *max_size;
};
typedef struct queue QUEUE;

/* Function prototypes */
int *create_service_points(int);
NODE *new_node(int);
QUEUE *initialise(int);
int is_empty(QUEUE *);
void enqueue(QUEUE *, int);
void dequeue(QUEUE *);
void print_queue(QUEUE *);

/* Main function */
int main()
{
    // Creates a random number of service points.
    srand(time(0));
    int points = rand() / 500;
    int *service_points = (int *)create_service_points(points);

    // Displays all the service points at the start.
    printf("Number of Service Points: %d\n", points);
    int j;
    for (j = 0; j < points; j++)
    {
        printf("Service Point: %d, Value: %d\n", j, service_points[j]);
    }

    // Creates the queue of customers.
    int max_size = 20;
    QUEUE *q = initialise(max_size);
    int k;
    for (k = 0; k < 20; k++)
    {
        srand(time(0));
        enqueue(q, k);
    }
    printf("Queue Front: %d\n", q->front->data);
    printf("Queue Rear: %d\n", q->rear->data);
    print_queue(q);

    free(q);
    return EXIT_SUCCESS;
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
        fprintf(stderr, "Insufficient memory.\n");
        exit(EXIT_FAILURE);
    };

    // Assigns each service point to represent them being empty.
    for (i = 0; i < points; i++)
    {
        service_points[i] = 0;
    }

    free(service_points);
    return service_points;
}

/* Creates a new linked list node to represent a person. */
NODE *new_node(int value)
{
    NODE *person = (NODE *)malloc(sizeof(NODE));
    if (person == NULL)
    {
        fprintf(stderr, "Insufficient memory.\n");
        exit(EXIT_FAILURE);
    }

    person->data = value;
    person->next = NULL;
    free(person);
    return person;
}

/* Creates an empty queue. */
QUEUE *initialise(int max_size)
{
    QUEUE *q = (QUEUE *)malloc(sizeof(QUEUE));
    if (q == NULL)
    {
        fprintf(stderr, "Insufficient memory.\n");
        exit(EXIT_FAILURE);
    }

    q->front = q->rear = NULL;
    q->size = 0;
    q->max_size = max_size;
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

/* Displays the full queue, alongside the details of people in the queue. */
void print_queue(QUEUE *q)
{
    NODE *person = q->front;
    int iterator = 0;
    while (person != NULL)
    {
        printf("Queue Node: %d\n   Data Value: %d\n   Pointer: %p\n   Next Pointer: %p\n", iterator, person->data, person, person->next);
        person = person->next;
        iterator += 1;
    }
}
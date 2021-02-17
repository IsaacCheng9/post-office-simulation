/* Queue implementation as a linked list. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Node structure using a linked list. */
struct node
{
    int hours;
    struct node *next;
};
typedef struct node NODE;

/* Queue structure in a linked list, with front and rear pointers. */
struct queue
{
    struct node *front, *rear;
    int size, max_size;
};
typedef struct queue QUEUE;

/* Function prototypes */
int *create_service_points(int);
NODE *new_node(int);
QUEUE *initialise(int);
int is_empty(QUEUE *);
void enqueue(QUEUE *, int);
int dequeue(int, QUEUE *);
int pop(QUEUE *);
void print_queue(QUEUE *);

/* Main function */
int main()
{
    /* Creates a random number of service points. */
    srand(time(0));
    /* int points = rand() / 500; */
    int points = 20;
    int *service_points = (int *)create_service_points(points);

    /* Displays all the service points at the start. */
    printf("Number of Service Points: %d\n", points);
    int j;
    for (j = 0; j < points; j++)
    {
        printf("Service Point: %d, Value: %d\n", j, service_points[j]);
    }

    /* Creates the queue of customers. */
    int max_size = 20;
    QUEUE *q = initialise(max_size);
    int k;
    for (k = 0; k < 2; k++)
    {
        enqueue(q, k);
    }
    printf("Queue Front: %d\n", q->front->hours);
    printf("Queue Rear: %d\n", q->rear->hours);
    print_queue(q);

    free(service_points);
    free(q);
    return EXIT_SUCCESS;
}

/* Other functions */

/* Creates the empty service points to start with. */
int *create_service_points(int points)
{
    int i;
    /* Dynamically allocates memory based on the number of service points. */
    int *service_points = NULL;
    if (!(service_points = (int *)malloc(points * sizeof(int))))
    {
        fprintf(stderr, "Insufficient memory.\n");
        exit(EXIT_FAILURE);
    };

    /* Assigns each service point to represent them being empty. */
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
    if (person == NULL)
    {
        fprintf(stderr, "Insufficient memory.\n");
        exit(EXIT_FAILURE);
    }

    person->hours = value;
    person->next = NULL;

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
    /* Creates a new node and its hours and next node pointer. */
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

    q->size++;
}

/* Removes a given value from the queue. */
int dequeue(int index, QUEUE *q)
{
    /* Iterator to traverse linked list until index reached. */
    int iterator = 0;

    /* Returns NULL if the queue is empty. */
    if (is_empty(q))
    {
        return 0;
    }

    /* Starts iterating from the front of the queue. */
    NODE *person = q->front;
    for (iterator = 0; iterator < index; iterator++)
    {
        person = person->next;
    }

    /* Stores the previous front of the queue, and moves the next person up. */
    int hours = person->hours;
    q->front = q->front->next;

    /* If this makes the front empty, make the rear empty, as this means the
    queue is empty. */
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(person);
    q->size--;
    return hours;
}

/* Dequeues a value from the front of the queue. */
int pop(QUEUE *q) {
    return dequeue(0, q);
}

/* Displays the full queue, alongside the details of people in the queue. */
void print_queue(QUEUE *q)
{
    NODE *person = q->front;
    int iterator = 0;
    while (person != NULL)
    {
        printf("\nQueue Node: %d\n   Hours: %d\n   Pointer: %p\n   Next Pointer: %p\n", iterator, person->hours, person, person->next);
        person = person->next;
        iterator += 1;
    }
    printf("\nQueue Size: %d\n", q->size);
}
/* Queue implementation as a linked list. */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Node structure using a linked list. */
struct node
{
    int hours, time_waited, tolerance;
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
int dequeue(QUEUE *);
void print_queue(QUEUE *);

/* Main function */
int main(int argc, char **argv)
{
    /* Creates a random number of service points. */
    srand(time(0));
    /* int points = rand() / 500; */
    int num_points = 20;
    int *service_points = (int *)create_service_points(num_points);

    /* Displays all the service points at the start. */
    printf("Number of Service Points: %d\n", num_points);
    int j;
    for (j = 0; j < num_points; j++)
    {
        printf("Service Point: %d, Value: %d\n", j, service_points[j]);
    }

    // Starts the simulation.
    int num_customers = atoi(argv[1]);
    int max_size = atoi(argv[2]);
    int simulation_length = atoi(argv[3]);
    int time_slice;
    /* Creates the queue of customers. */
    for (time_slice = 0; time_slice < simulation_length; time_slice++)
    {
        printf("Success!\n");
    }

    
    QUEUE *q = initialise(max_size);

    /* int k;
    for (k = 0; k < num_customers; k++)
    {
        enqueue(q, k);
    }
    printf("Queue Front: %d\n", q->front->hours);
    printf("Queue Rear: %d\n", q->rear->hours); */

    free(service_points);
    free(q);
    return EXIT_SUCCESS;
}

/* Other functions */

/* Creates the empty service points to start with. */
int *create_service_points(int num_points)
{
    int point;
    /* Dynamically allocates memory based on the number of service points. */
    int *service_points = NULL;
    if (!(service_points = (int *)malloc(num_points * sizeof(int))))
    {
        fprintf(stderr, "Insufficient memory.\n");
        exit(EXIT_FAILURE);
    };

    /* Assigns each service point to represent them being empty. */
    for (point = 0; point < num_points; point++)
    {
        service_points[point] = 0;
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
    person->time_waited = 0;
    person->tolerance = value;
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
int dequeue(QUEUE *q)
{
    /* Returns NULL if the queue is empty. */
    if (is_empty(q))
    {
        return 0;
    }

    /* Gets the previous front of the queue. */
    NODE *person = q->front;

    /* Stores the hours from the person, and moves the next person up. */
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

/* Displays the full queue, alongside the details of people in the queue. */
void print_queue(QUEUE *q)
{
    /* Starts from the front of the queue. */
    NODE *person = q->front;
    int iterator = 0;

    /* Iterates to print the details of each person in the queue. */
    while (person != NULL)
    {
        printf("\nQueue Node: %d\n   Hours: %d\n   Time Waited: %d\n   "
               "Tolerance: %d\n   Pointer: %p\n   Next Pointer: %p\n",
               iterator, person->hours, person->time_waited, person->tolerance,
               person, person->next);
        person = person->next;
        iterator += 1;
    }

    printf("\nQueue Size: %d\n", q->size);
}
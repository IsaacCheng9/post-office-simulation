/* Queue implementation as a linked list. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
NODE *create_new_node(int);
QUEUE *create_empty_queue(int);
int is_empty(QUEUE *);
void increment_waiting_times(QUEUE *);
void enqueue(QUEUE *, int);
int dequeue(QUEUE *);
int leave_queue_early(QUEUE *, int);
void print_queue(QUEUE *);

/* Main function */
int main(int argc, char **argv)
{
    /* int max_size = atoi(argv[1]);
    int num_service_points = atoi(argv[2]);
    int closing_time = atoi(argv[3]); */
    int max_size = 5;
    int num_service_points = 3;
    int closing_time = 10;
    int num_served = 0;
    int num_unfulfilled = 0;
    int num_timed_out = 0;
    int time_slice;
    /* Seeds for randomness. */
    srand(time(0));

    /* Creates service points and displays them at the start. */
    int *service_points = (int *)create_service_points(num_service_points);
    printf("Number of Service Points: %d\n", num_service_points);
    int point;
    for (point = 0; point < num_service_points; point++)
    {
        printf("Service Point: %d, Value: %d\n", point, service_points[point]);
    }

    /* Performs the simulation(s). */
    QUEUE *q = create_empty_queue(max_size);
    for (time_slice = 1; time_slice <= closing_time; time_slice++)
    {
        float new_cust_chance = 10.0 * (float)rand() / RAND_MAX;
        int new_hours = (int)new_cust_chance;
        printf("New Hours: %d\n", new_hours);
        if (new_cust_chance >= 5.0)
        {
            /* Adds customer to the queue if there is room. */
            if (q->size == max_size)
            {
                num_unfulfilled++;
            }
            else
            {
                enqueue(q, new_hours);
            }
        }

        increment_waiting_times(q);
        num_timed_out = leave_queue_early(q, num_timed_out);
    }
    print_queue(q);
    printf("People Unfulfilled: %d\n", num_unfulfilled);
    printf("People Timed Out: %d", num_timed_out);

    free(service_points);
    free(q);
    return EXIT_SUCCESS;
}

/* Other functions */

/* Creates the empty service points to start with. */
int *create_service_points(int num_service_points)
{
    int point;
    /* Dynamically allocates memory based on the number of service points. */
    int *service_points = NULL;
    if (!(service_points = (int *)malloc(num_service_points * sizeof(int))))
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    };

    /* Assigns each service point to represent them being empty. */
    for (point = 0; point < num_service_points; point++)
    {
        service_points[point] = 0;
    }

    return service_points;
}

/* Creates a new linked list node to represent a customer. */
NODE *create_new_node(int value)
{
    NODE *customer = (NODE *)malloc(sizeof(NODE));
    if (customer == NULL)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    customer->hours = value;
    customer->time_waited = 0;
    customer->tolerance = value;
    customer->next = NULL;

    return customer;
}

/* Creates an empty queue. */
QUEUE *create_empty_queue(int max_size)
{
    QUEUE *q = (QUEUE *)malloc(sizeof(QUEUE));
    if (q == NULL)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
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

/* Increments the waiting times of all people in the queue. */
void increment_waiting_times(QUEUE *q)
{
    /* Starts from the front of the queue. */
    NODE *customer = q->front;

    /* Iterates to print the details of each customer in the queue. */
    while (customer != NULL)
    {
        customer->time_waited++;
        customer = customer->next;
    }
}

/* Adds a value onto the end of the queue and increases queue count. */
void enqueue(QUEUE *q, int value)
{
    /* Creates a new node and its hours and next node pointer. */
    NODE *customer = create_new_node(value);

    /* Points front and rear of the queue to the new node if empty. Otherwise,
    points the previous rear of the node to this node, and sets the new node as
    the rear. */
    if (is_empty(q))
    {
        q->front = q->rear = customer;
        return;
    }
    else
    {
        q->rear->next = customer;
        q->rear = customer;
    }

    q->size++;
}

/* Removes a given value from the queue. */
int dequeue(QUEUE *q)
{
    /* Returns 0 if the queue is empty; there is no request to be handled. */
    if (is_empty(q))
    {
        return 0;
    }

    /* Gets the previous front of the queue. */
    NODE *customer = q->front;

    /* Stores the hours from the customer, and moves the next customer up. */
    int hours = customer->hours;
    q->front = q->front->next;

    /* If this makes the front empty, make the rear empty, as this means the
    queue is empty. */
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(customer);
    q->size--;
    return hours;
}

int leave_queue_early(QUEUE *q, int num_timed_out)
{
    /* Returns NULL if the queue is empty. */
    if (is_empty(q))
    {
        return num_timed_out;
    }
    
    /* Starts from the front of the queue. */
    NODE *customer = q->front;

    /* Iterates to check for customers in the queue who are leaving early. */
    while (customer != NULL)
    {
        /* Checks if customer has waited for longer than they will tolerate. */
        if (customer->time_waited > customer->tolerance)
        {
            /* Moves the next customer up. */
            q->front = q->front->next;

            /* If this makes the front empty, make the rear empty, as this
            means the queue is empty. */
            if (q->front == NULL)
            {
                q->rear = NULL;
            }

            num_timed_out++;
            q->size--;
        }
        customer = customer->next;
    }

    free(customer);
    return num_timed_out;
}

/* Displays the full queue, alongside the details of people in the queue. */
void print_queue(QUEUE *q)
{
    /* Starts from the front of the queue. */
    NODE *customer = q->front;
    int iterator = 0;

    /* Iterates to print the details of each customer in the queue. */
    while (customer != NULL)
    {
        printf("\nQueue Node: %d\n   Hours: %d\n   Time Waited: %d\n   "
               "Tolerance: %d\n   Pointer: %p\n   Next Pointer: %p\n",
               iterator, customer->hours, customer->time_waited, customer->tolerance,
               customer, customer->next);
        customer = customer->next;
        iterator += 1;
    }

    printf("\nQueue Size: %d\n", q->size);
}
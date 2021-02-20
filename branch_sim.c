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
    int queue_length, max_queue_length;
};
typedef struct queue QUEUE;

/* Function prototypes */
int *create_service_points(int);
NODE *create_new_node(int);
QUEUE *create_empty_queue(int);
int is_queue_empty(QUEUE *);
void increment_waiting_times(QUEUE *);
void enqueue(QUEUE *, int);
int dequeue(QUEUE *);
void fulfil_customer(QUEUE *, int, int *);
int serve_customers(int, int, int *);
int leave_queue_early(QUEUE *, int);
int is_branch_empty(QUEUE *, int, int *);
void print_queue(QUEUE *);

/* Main function */
int main(int argc, char **argv)
{
    /* int max_queue_length = atoi(argv[1]);
    int num_service_points = atoi(argv[2]);
    int closing_time = atoi(argv[3]); */
    int max_queue_length = 2;
    int num_service_points = 1;
    int closing_time = 10;
    int num_customers = 0;
    int num_fulfilled = 0;
    int num_unfulfilled = 0;
    int num_timed_out = 0;
    int time_slice;
    int closed = 0;
    /* Seeds for randomness. */
    srand(time(0));

    /* Creates service points and displays them at the start. */
    int *service_points = (int *)create_service_points(num_service_points);
    printf("Number of Service Points: %d\n\n", num_service_points);

    /* Performs the simulation(s). */
    QUEUE *q = create_empty_queue(max_queue_length);
    printf("Starting simulation.\n");
    time_slice = 1;
    while (closed == 0)
    {
        printf("Time Slice: %d\n", time_slice);

        /* Adds new customers to the queue if not past closing time. */
        if (time_slice <= closing_time)
        {
            float new_cust_chance = 10.0 * (float)rand() / RAND_MAX;
            int new_hours = (int)new_cust_chance;
            if (new_cust_chance >= 5.0)
            {
                num_customers++;
                printf("   New Hours: %d\n", new_hours);
                /* Marks the customer as unfulfilled if queue is full. */
                if (q->queue_length == max_queue_length)
                {
                    num_unfulfilled++;
                    printf("   Customer unfulfilled; queue is full.\n");
                }
                /* Adds customer to the queue if there is space. */
                else
                {
                    enqueue(q, new_hours);
                    printf("   Customer added to queue. Queue length is now %d"
                           ".\n",
                           q->queue_length);
                }
            }
        }

        /* Serves customers currently on the service points. */
        num_fulfilled = serve_customers(num_fulfilled, num_service_points,
                                        service_points);

        /* Checks if service points are available for the next customer. */
        if (!(is_queue_empty(q)))
        {
            fulfil_customer(q, num_service_points, service_points);
        }

        /* Updates the time waited of every customer in the queue. */
        increment_waiting_times(q);
        num_timed_out = leave_queue_early(q, num_timed_out);

        time_slice++;

        /* Checks if it can close the entire branch and stop the simulation. */
        if (time_slice > closing_time && is_branch_empty(q, num_service_points,
                                                         service_points))
        {
            closed = 1;
        }
    }
    print_queue(q);
    printf("\nNumber of Customers: %d\n   Customers Fulfilled: %d\n   "
           "Customers Unfulfilled: %d\n   Customers Timed Out: %d",
           num_customers, num_fulfilled, num_unfulfilled, num_timed_out);

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
    customer->tolerance = 3;
    customer->next = NULL;

    return customer;
}

/* Creates an empty queue. */
QUEUE *create_empty_queue(int max_queue_length)
{
    QUEUE *q = (QUEUE *)malloc(sizeof(QUEUE));
    if (q == NULL)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    q->front = q->rear = NULL;
    q->queue_length = 0;
    q->max_queue_length = max_queue_length;

    return q;
}

/* Checks if the queue is empty. */
int is_queue_empty(QUEUE *q)
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
    if (is_queue_empty(q))
    {
        q->front = q->rear = customer;
        q->queue_length++;
        return;
    }
    else
    {
        q->rear->next = customer;
        q->rear = customer;
        q->queue_length++;
    }
}

/* Removes a given value from the queue. */
int dequeue(QUEUE *q)
{
    /* Returns 0 if the queue is empty; there is no request to be handled. */
    if (is_queue_empty(q))
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
    q->queue_length--;
    return hours;
}

/* Attempts to service the customer at the front of the queue. */
void fulfil_customer(QUEUE *q, int num_service_points, int *service_points)
{
    /* Takes the customer from the front of the queue. */
    NODE *customer = q->front;

    /* Finds the first service point which is available. */
    int point;
    for (point = 0; point < num_service_points; point++)
    {
        if (service_points[point] == 0)
        {
            service_points[point] = customer->hours;
            dequeue(q);
            printf("   Fulfilled customer! Queue length is now %d.\n",
                   q->queue_length);
            return;
        }
    }

    printf("   No service points free!\n");
}

/* Processes customers being served for that time slice. */
int serve_customers(int num_fulfilled, int num_service_points, int *service_points)
{
    /* Iterates to check for service points which are being used. */
    int point;
    for (point = 0; point < num_service_points; point++)
    {
        if (service_points[point] != 0)
        {
            service_points[point]--;
            /* Checks if a customer has been fully served. */
            if (service_points[point] == 0)
            {
                printf("   Finished serving customer!\n");
                num_fulfilled++;
            }
        }
    }

    return num_fulfilled;
}

/* Removes people who have waited for too long from the queue. */
int leave_queue_early(QUEUE *q, int num_timed_out)
{
    /* Returns NULL if the queue is empty. */
    if (is_queue_empty(q))
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
            q->queue_length--;
            printf("   Customer has waited too long, and has left the queue "
                   "early. Queue length is now %d.\n",
                   q->queue_length);
        }
        customer = customer->next;
    }

    free(customer);
    return num_timed_out;
}

/* Displays the full queue, with the details of each person in the queue. */
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
               iterator, customer->hours, customer->time_waited,
               customer->tolerance, customer, customer->next);
        customer = customer->next;
        iterator += 1;
    }
}

int is_branch_empty(QUEUE *q, int num_service_points, int *service_points)
{
    /* Queue must be empty for the branch to be empty. */
    if (is_queue_empty)
    {
        /* Iterates to check that no service points are busy. */
        int point;
        for (point = 0; point < num_service_points; point++)
        {
            if (service_points[point] != 0)
            {
                return 0;
            }
        }

        /* Branch is empty if the queue and all service points are empty. */
        return 1;
    }
}
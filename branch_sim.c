/* Simulation of the queuing system in a Post Office branch. */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Node structure using a linked list. */
struct node
{
    int mins, time_waited, tolerance;
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
int *read_parameter_file(char *);
int *create_service_points(int);
NODE *create_new_node(int);
QUEUE *create_empty_queue(int);
int count_busy_service_points(int, int *);
int is_queue_empty(QUEUE *);
void increment_waiting_times(QUEUE *);
void enqueue(QUEUE *, int);
int dequeue(QUEUE *);
int fulfil_customer(QUEUE *, int, int *, int);
int serve_customers(int, int, int *);
int leave_queue_early(QUEUE *, int);
int is_branch_empty(QUEUE *, int, int *);
void print_queue(QUEUE *);
void output_interval_record(char *, int, int, int, int, int, int, int);
void output_results_mult(char *, int, int, int, int, int, int, int, int, int, int);

/* Main function */
int main(int argc, char **argv)
{
    /* Takes the configuration from the parameters. */
    char *input_parameters = argv[1];
    int num_simulations = atoi(argv[2]);
    char *results_file = argv[3];
    int *parameters = (int *)read_parameter_file(input_parameters);

    /* Configuration variables. */
    int max_queue_length = parameters[0];
    int num_service_points = parameters[1];
    int closing_time = parameters[2];

    /* Variables for the running/output of the simulations. */
    int simulation;
    int num_customers = 0;
    int num_fulfilled = 0;
    int num_unfulfilled = 0;
    int num_timed_out = 0;
    int fulfilled_wait_time = 0;
    int time_after_closing = 0;
    int time_slice;
    int closed = 0;

    /* Seeds for randomness. */
    srand(time(0));

    /* Creates service points and displays them at the start. */
    int *service_points = (int *)create_service_points(num_service_points);
    printf("Number of Service Points: %d\n", num_service_points);

    for (simulation = 0; simulation < num_simulations; simulation++)
    {
        /* Performs the simulation(s). */
        QUEUE *q = create_empty_queue(max_queue_length);
        printf("\nStarting simulation #%d:\n", simulation);
        time_slice = 0;
        closed = 0;
        while (closed == 0)
        {
            printf("Time Slice: %d\n", time_slice);

            /* Adds new customers to the queue if not past closing time. */
            if (time_slice <= closing_time)
            {
                float new_cust_chance = 10.0 * (float)rand() / RAND_MAX;
                int new_mins = (int)new_cust_chance;
                if (new_cust_chance >= 5.0)
                {
                    num_customers++;
                    printf("   New Mins: %d\n", new_mins);
                    /* Marks the customer as unfulfilled if queue is full. */
                    if (q->queue_length == max_queue_length)
                    {
                        num_unfulfilled++;
                        printf("   Customer unfulfilled; queue is full.\n");
                    }
                    /* Adds customer to the queue if there is space. */
                    else
                    {
                        enqueue(q, new_mins);
                        printf("   Customer added to queue. Queue length is "
                               "now %d.\n",
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
                fulfilled_wait_time = fulfil_customer(q, num_service_points,
                                                      service_points,
                                                      fulfilled_wait_time);
            }

            /* Updates the time waited of every customer in the queue. */
            increment_waiting_times(q);
            num_timed_out = leave_queue_early(q, num_timed_out);

            /* Displays a record for each time interval. */
            if (num_simulations == 1)
            {
                int num_being_served = count_busy_service_points(
                    num_service_points, service_points);
                output_interval_record(results_file, time_slice, closing_time,
                                       num_being_served, q->queue_length,
                                       num_fulfilled, num_unfulfilled,
                                       num_timed_out);
            }

            /* Stops the simulation. */
            time_slice++;
            if (time_slice > closing_time &&
                is_branch_empty(q, num_service_points, service_points))
            {
                time_after_closing += time_slice - closing_time - 1;
                closed = 1;
                free(q);
            }
        }
    }

    /* Outputs to the results file for multiple simulations. */
    if (num_simulations > 1)
    {
        output_results_mult(results_file, num_simulations, max_queue_length,
                            num_service_points, closing_time, num_customers,
                            num_fulfilled, fulfilled_wait_time,
                            num_unfulfilled, num_timed_out,
                            time_after_closing);
    }

    free(service_points);
    return EXIT_SUCCESS;
}

/* Other functions */

/* Reads a file to get parameters for the simulation. */
int *read_parameter_file(char *input_parameters)
{
    FILE *fp;

    /* Allocates memory to store the parameters. */
    int *parameters = NULL;
    if (!(parameters = (int *)malloc(3 * sizeof(int))))
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    };

    /* Opens the parameter file to read from it. */
    if ((fp = fopen(input_parameters, "r")) == NULL)
    {
        printf("Unable to open file for read access.\n");
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* char *line_buf = NULL;
    size_t line_buf_size = 0;
    int line_count = 0;
    size_t line_size = getline(&line_buf, &line_buf_size, fp);
    while (line_size >= 0)
    {
        line_count++;
    } */

    /* Searches for the parameters in the text file. */
    fscanf(fp, "\nmaxQueueLength %d", &parameters[0]);
    fscanf(fp, "\nnumServicePoints %d", &parameters[1]);
    fscanf(fp, "\nclosingTime %d", &parameters[2]);

    printf("Parameter 0: %d\nParameter 1: %d\nParameter 2: %d\n",
           parameters[0], parameters[1], parameters[2]);

    fclose(fp);
    return parameters;
}

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

    customer->mins = (int)10.0 * (float)rand() / RAND_MAX;
    customer->time_waited = 0;
    customer->tolerance = (int)10.0 * (float)rand() / RAND_MAX;
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

/* Counts the number of people being served in the service points. */
int count_busy_service_points(int num_service_points, int *service_points)
{
    int num_being_served = 0;

    /* Iterates to check for service points which are being used. */
    int point;
    for (point = 0; point < num_service_points; point++)
    {
        if (service_points[point] != 0)
        {
            num_being_served++;
        }
    }

    return num_being_served;
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
    /* Creates a new node and its mins and next node pointer. */
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

    /* Stores the mins from the customer, and moves the next customer up. */
    int mins = customer->mins;
    q->front = q->front->next;

    /* If this makes the front empty, make the rear empty, as this means the
    queue is empty. */
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(customer);
    q->queue_length--;
    return mins;
}

/* Attempts to service the customer at the front of the queue. */
int fulfil_customer(QUEUE *q, int num_service_points, int *service_points,
                    int fulfilled_wait_time)
{
    /* Takes the customer from the front of the queue. */
    NODE *customer = q->front;

    /* Finds the first service point which is available. */
    int point;
    for (point = 0; point < num_service_points; point++)
    {
        if (service_points[point] == 0)
        {
            service_points[point] = customer->mins;
            fulfilled_wait_time += customer->time_waited;
            dequeue(q);
            printf("   Fulfilled customer! Queue length is now %d.\n",
                   q->queue_length);
            return fulfilled_wait_time;
        }
    }

    /* All service points may be in use. */
    printf("   No service points free!\n");
    return fulfilled_wait_time;
}

/* Processes customers being served for that time slice. */
int serve_customers(int num_fulfilled, int num_service_points,
                    int *service_points)
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
        if (customer->time_waited == customer->tolerance)
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
        printf("\nQueue Node: %d\n   Mins: %d\n   Time Waited: %d\n   "
               "Tolerance: %d\n   Pointer: %p\n   Next Pointer: %p\n",
               iterator, customer->mins, customer->time_waited,
               customer->tolerance, customer, customer->next);
        customer = customer->next;
        iterator += 1;
    }
}

/* Checks if it can close the entire branch so it can stop the simulation. */
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

/* Outputs live information about the simulation for a given time interval. */
void output_interval_record(char *results_file, int time_slice,
                            int closing_time, int num_being_served,
                            int queue_length, int num_fulfilled,
                            int num_unfulfilled, int num_timed_out)
{
    FILE *fp;

    /* Opens file in write mode to overwrite old contents. */
    if (time_slice == 0)
    {
        /* Error handling for opening the file in write mode. */
        if ((fp = fopen(results_file, "w+")) == NULL)
        {
            printf("Unable to open file for write access.\n");
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    }
    else
    {
        /* Error handling for opening the file in append mode. */
        if ((fp = fopen(results_file, "a")) == NULL)
        {
            printf("Unable to open file for append access.\n");
            fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    }

    fprintf(fp, "Time Slice: %d\n   Number of Customers Currently Being "
                "Served: %d\n   Number of People Currently in the Queue: "
                "%d\n   Number of Fulfilled Customers: %d\n   Number of "
                "Unfilfilled Customers: %d\n   Number of Timed Out "
                "Customers: %d\n\n",
            time_slice, num_being_served, queue_length,
            num_fulfilled, num_unfulfilled, num_timed_out);

    if (time_slice == closing_time)
    {
        fprintf(fp, "Closing time has been reached!\n\n");
    }
}

/* Outputs statistics about averages in a file for multiple simulations. */
void output_results_mult(char *results_file, int num_simulations, int max_queue_length,
                         int num_service_points, int closing_time,
                         int num_customers, int num_fulfilled,
                         int fulfilled_wait_time, int num_unfulfilled,
                         int num_timed_out, int time_after_closing)
{
    FILE *fp;

    /* Error handling for opening the file in write mode. */
    if ((fp = fopen(results_file, "w+")) == NULL)
    {
        printf("Unable to open file for write access.\n");
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    fprintf(fp, "Number of Simulations: %d\n\nParameters Read From Input "
                "File:\n   Max Queue Length: %d\n   "
                "Number of Service Points: %d\n   Closing Time: %d\n\n"
                "Average Number of Customers Fulfilled: %f\n"
                "Average Number of Customers Unfulfilled: %f\n"
                "Average Number of Customers Timed Out: %f\n"
                "Average Waiting Time for Fulfilled Customers: %f\n"
                "Average Time After Closing to Finish Serving Remaining "
                "Customers: %f",
            num_simulations, max_queue_length, num_service_points,
            closing_time, (float)num_fulfilled / num_simulations,
            (float)num_unfulfilled / num_simulations,
            (float)num_timed_out / num_simulations,
            (float)fulfilled_wait_time / num_fulfilled,
            (float)time_after_closing / num_simulations);
}
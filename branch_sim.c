/* Simulation of the queuing system in a Post Office branch using linked
lists. */
#include <errno.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Customer structure using a linked list, acting as a node. */
struct customer
{
    int mins, time_waited, tolerance;
    struct customer *next;
};
typedef struct customer CUSTOMER;

/* Queue structure in a linked list, with front and rear pointers. */
struct queue
{
    struct customer *front, *rear;
    int queue_length, max_queue_length;
};
typedef struct queue QUEUE;

/* Function prototypes */
int generate_random_poisson(float, gsl_rng *);
int generate_random_gaussian(int, int, gsl_rng *);
float *read_parameter_file(char *);
int *create_service_points(int);
CUSTOMER *create_new_customer(int, int, int, int, gsl_rng *);
QUEUE *create_empty_queue(int);
int count_busy_service_points(int, int *);
int is_queue_empty(QUEUE *);
void increment_waiting_times(QUEUE *);
void enqueue(QUEUE *, int, int, int, int, gsl_rng *);
int dequeue(QUEUE *);
int fulfil_customer(QUEUE *, int, int *, int);
int serve_customers(int, int, int *);
int leave_queue_early(QUEUE *, int);
int is_branch_empty(QUEUE *, int, int *);
void print_queue(QUEUE *);
void output_parameters(char *, int, int, int, float, float, float, float,
                       float);
void output_interval_record(char *, int, int, int, int, int, int, int);
void output_results_sing(char *, int, int, int);
void output_results_mult(char *, int, int, int, int, int, int, int);

/* Main function */
int main(int argc, char **argv)
{
    const gsl_rng_type *T;
    gsl_rng *r;

    /* Creates a random number generator. */
    gsl_rng_env_setup();
    T = gsl_rng_default;
    r = gsl_rng_alloc(T);

    /* Seeds the random number generator based on current time. */
    gsl_rng_set(r, time(0));

    /* Checks that enough parameters have been passed into the program. */
    if (argc != 4)
    {
        fprintf(stderr, "Not enough parameters passed in! You must provide "
                        "the input file, number of simulations, and "
                        "output file.");
        exit(EXIT_FAILURE);
    }

    /* Checks that the second parameter input by the user is a digit. */
    if (!isdigit(*argv[2]))
    {
        fprintf(stderr, "You have not input a digit for the number of "
                        "simulations!");
        exit(EXIT_FAILURE);
    }

    /* Takes the configuration from the parameters. */
    char *input_parameters = argv[1];
    int num_simulations = atoi(argv[2]);
    char *results_file = argv[3];
    float *parameters = (float *)read_parameter_file(input_parameters);

    /* Configuration variables from the input file. */
    int max_queue_length = parameters[0];
    int num_service_points = parameters[1];
    int closing_time = parameters[2];
    float avg_customer_rate = parameters[3];
    float mean_mins = parameters[4];
    float std_dev_mins = parameters[5];
    float mean_tolerance = parameters[6];
    float std_dev_tolerance = parameters[7];

    /* Removes queue length limit if set to -1. */
    if (max_queue_length == -1)
    {
        max_queue_length = INT_MAX;
    }

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

    /* Outputs parameter values. */
    output_parameters(results_file, max_queue_length, num_service_points,
                      closing_time, avg_customer_rate, mean_mins, std_dev_mins,
                      mean_tolerance, std_dev_tolerance);

    /* Creates service points and displays them at the start. */
    int *service_points = (int *)create_service_points(num_service_points);

    for (simulation = 0; simulation < num_simulations; simulation++)
    {
        /* Performs the simulation(s). */
        QUEUE *q = create_empty_queue(max_queue_length);
        printf("\nStarting simulation #%d:\n", simulation);
        time_slice = 0;
        closed = 0;
        while (closed == 0)
        {
            printf("\nTime Slice: %d\n", time_slice);

            /* Adds new customers to the queue if not past closing time. */
            if (time_slice <= closing_time)
            {
                int new_customer;
                int num_new_customers = generate_random_poisson(
                    avg_customer_rate, r);
                for (new_customer = 0; new_customer < num_new_customers;
                     new_customer++)
                {
                    num_customers++;
                    /* Marks the customer as unfulfilled if queue is full. */
                    if (q->queue_length == max_queue_length)
                    {
                        num_unfulfilled++;
                        printf("   Customer left unfulfilled; queue is "
                               "full.\n");
                    }
                    /* Adds customer to the queue if there is space. */
                    else
                    {
                        enqueue(q, mean_mins, std_dev_mins, mean_tolerance,
                                std_dev_tolerance, r);
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
    if (num_simulations == 1)
    {
        output_results_sing(results_file, time_after_closing, num_fulfilled,
                            fulfilled_wait_time);
    }
    /* Outputs to the results file for multiple simulations. */
    else if (num_simulations > 1)
    {
        output_results_mult(results_file, num_simulations, num_customers,
                            num_fulfilled, fulfilled_wait_time,
                            num_unfulfilled, num_timed_out,
                            time_after_closing);
    }

    gsl_rng_free(r);
    free(parameters);
    free(service_points);
    return EXIT_SUCCESS;
}

/* Other functions */

/* Generates a random numberator using Gaussian distribution. */
int generate_random_poisson(float avg_customer_rate, gsl_rng *r)
{
    int random = -1;

    /* Creates a random number using the Poisson distribution based on
    the rate parameter. */
    random = gsl_ran_poisson(r, avg_customer_rate);

    return random;
}

/* Generates a random numberator using Gaussian distribution. */
int generate_random_gaussian(int mean, int std_dev, gsl_rng *r)
{
    int random = -1;

    /* Creates a random number >= 0 using the Gaussian distribution based on
    mean and standard deviation. */
    while (random < 0)
        random = gsl_ran_gaussian(r, std_dev) + mean;

    return random;
}

/* Reads a file to get parameters for the simulation. */
float *read_parameter_file(char *input_parameters)
{
    FILE *fp;

    /* Allocates memory to store the parameters. */
    float *parameters = NULL;
    if (!(parameters = (float *)malloc(9 * sizeof(float))))
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

    /* Searches for the parameters in the text file. */
    fscanf(fp, "\nmaxQueueLength %f", &parameters[0]);
    fscanf(fp, "\nnumServicePoints %f", &parameters[1]);
    fscanf(fp, "\nclosingTime %f", &parameters[2]);
    fscanf(fp, "\naverageCustomersPerMinute %f", &parameters[3]);
    fscanf(fp, "\nmeanMinsPerCustomerTask %f", &parameters[4]);
    fscanf(fp, "\nstandardDeviationMinsPerCustomerTask %f", &parameters[5]);
    fscanf(fp, "\nmeanMaxQueueTimePerCustomer %f", &parameters[6]);
    fscanf(fp, "\nstandardDeviationMaxQueueTimePerCustomer %f",
           &parameters[7]);

    /* Checks that parameters have valid values. */
    if (&parameters[0] < 0 || &parameters[1] < 1 || &parameters[2] < 1 ||
        &parameters[3] < 0 || &parameters[4] < 0 || &parameters[5] < 0 ||
        &parameters[6] < 0 || &parameters[7] < 0)
    {
        fprintf(stderr, "You have input an invalid parameter value! "
        "\nmaxQueueLength, averageCustomersPerMinute, meanMinsPerCustomerTask, "
        "standardDeviationMinsPerCustomerTask, meanMaxQueueTimePerCustomer, "
        "and standardDeviationMaxQueueTimePerCustomer must be at least 0."
        "\nnumServicePoints and closingTime must be at least 1.");
        exit(EXIT_FAILURE);
    }

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
CUSTOMER *create_new_customer(int mean_mins, int std_dev_mins,
                              int mean_tolerance, int std_dev_tolerance,
                              gsl_rng *r)
{
    CUSTOMER *customer = (CUSTOMER *)malloc(sizeof(CUSTOMER));
    if (customer == NULL)
    {
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    customer->mins = generate_random_gaussian(mean_mins, std_dev_mins, r);
    customer->time_waited = 0;
    customer->tolerance = generate_random_gaussian(mean_tolerance,
                                                   std_dev_tolerance, r);
    customer->next = NULL;

    return customer;
}

/* Creates an empty queue for customers to join. */
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
    CUSTOMER *customer = q->front;

    /* Iterates to print the details of each customer in the queue. */
    while (customer != NULL)
    {
        customer->time_waited++;
        customer = customer->next;
    }
}

/* Adds a value onto the end of the queue and increases queue count. */
void enqueue(QUEUE *q, int mean_mins, int std_dev_mins, int mean_tolerance,
             int std_dev_tolerance, gsl_rng *r)
{
    /* Creates a new customer and its mins and next customer pointer. */
    CUSTOMER *customer = create_new_customer(mean_mins, std_dev_mins,
                                             mean_tolerance,
                                             std_dev_tolerance, r);

    /* Points front and rear of the queue to the new customer if empty.
    Otherwise, points the previous rear of the customer to this customer, and
    sets the new customer as the rear. */
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
    CUSTOMER *customer = q->front;

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
    CUSTOMER *customer = q->front;

    /* Finds the first service point which is available. */
    int point;
    for (point = 0; point < num_service_points; point++)
    {
        if (service_points[point] == 0)
        {
            service_points[point] = customer->mins;
            fulfilled_wait_time += customer->time_waited;
            printf("   Fulfilled customer! They spent %d minutes waiting in "
                   "the queue. Their task will take %d minutes. Queue length "
                   "is now %d.\n",
                   customer->time_waited, customer->mins, q->queue_length - 1);
            dequeue(q);
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
    CUSTOMER *customer = q->front;

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
            printf("   Customer has timed out. They waited too long, and left "
                   "the queue early after %d minutes. Queue length is now "
                   "%d.\n",
                   customer->time_waited, q->queue_length);
        }
        customer = customer->next;
    }

    free(customer);
    return num_timed_out;
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

/* Outputs list of parameter values read from the input file. */
void output_parameters(char *results_file, int max_queue_length,
                       int num_service_points, int closing_time,
                       float avg_customer_rate, float mean_mins,
                       float std_dev_mins, float mean_tolerance,
                       float std_dev_tolerance)
{
    FILE *fp;

    /* Error handling for opening the file in append mode. */
    if ((fp = fopen(results_file, "w")) == NULL)
    {
        printf("Unable to open file for append access.\n");
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    fprintf(fp, "Parameters Read From Input File:\n   Max Queue Length: "
                "%d\n   Number of Service Points: %d\n   Closing Time: %d\n"
                "   Average Customers Per Interval: %f\n   Mean of Task "
                "Length: %f\n   Standard Deviation of Task Length: %f\n   "
                "Mean of Customer Tolerance: %f\n   Standard Deviation of "
                "Customer Tolerance: %f\n\n",
            max_queue_length, num_service_points, closing_time,
            avg_customer_rate, mean_mins, std_dev_mins, mean_tolerance,
            std_dev_tolerance);

    fclose(fp);
}

/* Outputs live information about the simulation for a given time interval. */
void output_interval_record(char *results_file, int time_slice,
                            int closing_time, int num_being_served,
                            int queue_length, int num_fulfilled,
                            int num_unfulfilled, int num_timed_out)
{
    FILE *fp;

    /* Error handling for opening the file in append mode. */
    if ((fp = fopen(results_file, "a")) == NULL)
    {
        printf("Unable to open file for append access.\n");
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    fprintf(fp, "Time Slice: %d\n   Number of Customers Currently Being "
                "Served: %d\n   Number of People Currently in the Queue: "
                "%d\n   Number of Fulfilled Customers: %d\n   Number of "
                "Unfulfilled Customers: %d\n   Number of Timed Out "
                "Customers: %d\n\n",
            time_slice, num_being_served, queue_length,
            num_fulfilled, num_unfulfilled, num_timed_out);

    if (time_slice == closing_time)
    {
        fprintf(fp, "Closing time has been reached!\n\n");
    }

    fclose(fp);
}

/* Outputs statistics about averages in a file for a single simulation. */
void output_results_sing(char *results_file, int time_after_closing,
                         int num_fulfilled, int fulfilled_wait_time)
{
    FILE *fp;

    /* Error handling for opening the file in append mode. */
    if ((fp = fopen(results_file, "a")) == NULL)
    {
        printf("Unable to open file for append access.\n");
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    fprintf(fp, "Time After Closing to Finish Serving Remaining Customers: "
                "%d\nAverage Waiting Time of Fulfilled Customers: %f\n",
            time_after_closing,
            (float)fulfilled_wait_time / num_fulfilled);

    fclose(fp);
}

/* Outputs statistics about averages in a file for multiple simulations. */
void output_results_mult(char *results_file, int num_simulations,
                         int num_customers, int num_fulfilled,
                         int fulfilled_wait_time, int num_unfulfilled,
                         int num_timed_out, int time_after_closing)
{
    FILE *fp;

    /* Error handling for opening the file in write mode. */
    if ((fp = fopen(results_file, "a")) == NULL)
    {
        printf("Unable to open file for write access.\n");
        fprintf(stderr, "Error %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    fprintf(fp, "Average Number of Customers Fulfilled: %f\n"
                "Average Number of Customers Unfulfilled: %f\n"
                "Average Number of Customers Timed Out: %f\n"
                "Average Waiting Time of Fulfilled Customers: %f\n"
                "Average Time After Closing to Finish Serving Remaining "
                "Customers: %f",
            (float)num_fulfilled / num_simulations,
            (float)num_unfulfilled / num_simulations,
            (float)num_timed_out / num_simulations,
            (float)fulfilled_wait_time / num_fulfilled,
            (float)time_after_closing / num_simulations);

    fclose(fp);
}
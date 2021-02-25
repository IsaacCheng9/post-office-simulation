/* Simulation of the queuing system in a Post Office branch using linked
 * lists. */
#include <simQ.h>

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


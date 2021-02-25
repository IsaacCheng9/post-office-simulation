/* Handles input and output. */
#include <input_output.h>

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
    if (parameters[0] < 0 || parameters[1] < 1 || parameters[2] < 1 ||
        parameters[3] < 0 || parameters[4] < 0 || parameters[5] < 0 ||
        parameters[6] < 0 || parameters[7] < 0)
    {
        fprintf(stderr, "You have input an invalid parameter value! "
                        "\nmaxQueueLength, averageCustomersPerMinute, "
                        "meanMinsPerCustomerTask, "
                        "standardDeviationMinsPerCustomerTask, "
                        "meanMaxQueueTimePerCustomer, and "
                        "standardDeviationMaxQueueTimePerCustomer must be at "
                        "least 0.\nnumServicePoints and closingTime must be "
                        "at least 1.");
        exit(EXIT_FAILURE);
    }

    fclose(fp);
    return parameters;
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

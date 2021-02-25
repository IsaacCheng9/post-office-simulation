/* Creates new customers. */
#include <customer.h>

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


/* Generates random numbers using Poisson and Gaussian distributions. */
#include <random_numbers.h>

/* Generates a random numberator using Gaussian distribution. */
int generate_random_poisson(float avg_customer_rate, gsl_rng *r)
{
    int random;

    /* Creates a random number using the Poisson distribution based on
 *     the rate parameter. */
    random = gsl_ran_poisson(r, avg_customer_rate);

    return random;
}

/* Generates a random numberator using Gaussian distribution. */
int generate_random_gaussian(int mean, int std_dev, gsl_rng *r)
{
    int random = -1;

    /* Creates a random number >= 0 using the Gaussian distribution based on
 *     mean and standard deviation. */
    while (random < 0)
        random = gsl_ran_gaussian(r, std_dev) + mean;

    return random;
}


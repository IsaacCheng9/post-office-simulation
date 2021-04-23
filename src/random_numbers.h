/* Header file for generating random numbers using Poisson and Gaussian
distributions. */
#ifndef __RANDOM_NUMBERS_H
#define __RANDOM_NUMBERS_H

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Random number generation functions. */
int generate_random_poisson(float, gsl_rng *);
int generate_random_gaussian(int, int, gsl_rng *);

#endif

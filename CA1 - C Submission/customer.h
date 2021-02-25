/* Header file for creating new customers. */
#ifndef __CUSTOMER_H
#define __CUSTOMER_H

#include <errno.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Customer structure using a linked list, acting as a node. */
struct customer
{
    int mins, time_waited, tolerance;
    struct customer *next;
};
typedef struct customer CUSTOMER;

/* Customer function prototypes. */
CUSTOMER *create_new_customer(int, int, int, int, gsl_rng *);

#endif


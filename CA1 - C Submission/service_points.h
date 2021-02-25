/* Header file for handling service points in a Post Office branch. */
#ifndef __SERVICE_POINTS_H
#define __SERVICE_POINTS_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Service point function prototypes. */
int *create_service_points(int);
int count_busy_service_points(int, int *);

#endif


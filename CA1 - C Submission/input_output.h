/* Header file for handling input and output. */
#ifndef __INPUT_OUTPUT_H
#define __INPUT_OUTPUT_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Input output function prototypes. */
float *read_parameter_file(char *);
void output_parameters(char *, int, int, int, float, float, float, float,
                       float);
void output_interval_record(char *, int, int, int, int, int, int, int);
void output_results_sing(char *, int, int, int);
void output_results_mult(char *, int, int, int, int, int, int, int);

#endif


/* Handles service points in a Post Office branch. */
#include <service_points.h>

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


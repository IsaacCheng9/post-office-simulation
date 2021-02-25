/* Handles the queue, which is implemented using linked lists. */
#include <queue.h>

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
 *     Otherwise, points the previous rear of the customer to this customer, and
 *         sets the new customer as the rear. */
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
 *     queue is empty. */
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
            dequeue(q);
            return fulfilled_wait_time;
        }
    }

    /* All service points may be in use. */
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
 *             means the queue is empty. */
            if (q->front == NULL)
            {
                q->rear = NULL;
            }

            num_timed_out++;
            q->queue_length--;
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


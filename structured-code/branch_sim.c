#include <branch_sim.h>

/* Main function */
int main()
{
    QUEUE *q = initialise();
    enqueue(q, 10);
    enqueue(q, 20);
    dequeue(q);
    dequeue(q);
    enqueue(q, 30);
    enqueue(q, 40);
    enqueue(q, 50);
    dequeue(q);
    printf("Queue Front: %d\n", q->front->data);
    printf("Queue Rear: %d\n", q->rear->data);

    return 0;
}
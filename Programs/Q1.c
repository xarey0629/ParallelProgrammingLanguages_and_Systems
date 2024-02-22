#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Data shared by the thread(s) */
int arrSize;
int avg;
int minVal;
int maxVal;

/* thread 1 */
void *avgCal(void *arg)
{
    int sum = 0;
    int *num = (int *)arg;
    for (int i = 0; i < arrSize; i++)
    {
        sum += num[i];
    }

    avg = sum / arrSize;
    pthread_exit(0);
}

/* thread 2 */
void *minCal(void *arg)
{
    int min = INT32_MAX;
    int *num = (int *)arg;
    for (int i = 0; i < arrSize; i++)
    {
        if (num[i] < min)
            min = num[i];
    }
    minVal = min;
    pthread_exit(0);
}

/* thread 3 */
void *maxCal(void *arg)
{
    int max = INT32_MIN;
    int *num = (int *)arg;
    for (int i = 0; i < arrSize; i++)
    {
        if (num[i] > max)
            max = num[i];
    }

    maxVal = max;
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2, tid3; /* the thread identifier */
    pthread_attr_t attr;        /* set of attributes for the thread */

    if (argc <= 1)
    {
        fprintf(stderr, "No input number detected\n");
        return -1;
    }

    arrSize = argc - 1;
    int *num = malloc(sizeof(int) * arrSize);
    for (int i = 1; i < argc; i++)
    {
        num[i - 1] = atoi(argv[i]);
    }

    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create the thread */
    pthread_create(&tid1, &attr, avgCal, num);
    pthread_create(&tid2, &attr, minCal, num);
    pthread_create(&tid3, &attr, maxCal, num);

    /* now wait for the thread to exit */
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    printf("The average value is %d\n", avg);
    printf("The minimum value is %d\n", minVal);
    printf("The maximum value is %d\n", maxVal);

    return 0;
}

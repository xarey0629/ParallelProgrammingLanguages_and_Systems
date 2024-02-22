#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

pthread_mutex_t mutex;

/* Data shared by the thread(s) */
int sharedCounts = 0;
int total = 5000;

void *calPiByRandom(void *params)
{
    // Entry
    pthread_mutex_lock(&mutex);

    double x = 0;
    double y = 0;

    for (int i = 0; i < 1000; i++)
    {
        /* code */
        x = (double)rand() / RAND_MAX;
        y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1)
        {
            sharedCounts++;
        }
    }
    // Exit
    pthread_mutex_unlock(&mutex);
    pthread_exit(0);
}

int main()
{
    // Random
    srand(time(NULL));

    // Mutex Initialize
    pthread_mutex_init(&mutex, NULL);

    pthread_t tid;       /* the thread identifier */
    pthread_attr_t attr; /* set of attributes for the thread */

    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create the thread */
    for (int i = 0; i < 5; i++)
    {
        void *foo;
        pthread_create(&tid, &attr, calPiByRandom, foo);
    }

    /* now wait for the two threads to exit */
    for (int i = 0; i < 5; i++)
    {
        /* code */
        pthread_join(tid, NULL);
    }

    // Main thread
    double PI = 4 * (double)sharedCounts / total;
    printf("PI calculated from the Monte Carlo random method is: %.5f", PI);

    return 0;
}

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int start;
    int end;
} parameters;

/* Data shared by the thread(s) */
int *num;
int arrSize;

void *sortHalf(void *params)
{
    parameters *ptr = (parameters *)params;
    for (int i = ptr->start + 1; i <= ptr->end; i++)
    {
        int tmp = num[i];
        int j = i - 1;
        while (j >= ptr->start)
        {
            if (num[j] > num[j + 1])
            {
                num[j + 1] = num[j];
                j--;
            }
            else
                break;
            num[j + 1] = tmp;
        }
    }
    pthread_exit(0);
}

void *myMergeSort(void *params)
{
    parameters *ptr = (parameters *)params;
    int id = 0;
    int tmpArr[arrSize];
    int l = ptr->start, r = arrSize / 2;

    while (l <= arrSize / 2 - 1 && r <= arrSize - 1)
    {
        if (num[l] < num[r])
        {
            tmpArr[id] = num[l];
            l++;
        }
        else
        {
            tmpArr[id] = num[r];
            r++;
        }
        id++;
    }
    if (l > arrSize / 2 - 1)
    {
        while (r <= arrSize - 1)
        {
            tmpArr[id] = num[r];
            r++, id++;
        }
    }
    else
    {
        while (l <= arrSize / 2 - 1)
        {
            tmpArr[id] = num[l];
            l++, id++;
        }
    }

    for (int i = 0; i < arrSize; i++)
    {
        num[i] = tmpArr[i];
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{
    pthread_t tid;       /* the thread identifier */
    pthread_attr_t attr; /* set of attributes for the thread */

    if (argc <= 1)
    {
        fprintf(stderr, "No input number detected\n");
        return -1;
    }

    arrSize = argc - 1;
    num = malloc(sizeof(int) * arrSize);
    for (int i = 1; i < argc; i++)
    {
        num[i - 1] = atoi(argv[i]);
    }

    /* get the default attributes */
    pthread_attr_init(&attr);

    /* create the thread */
    parameters *firsthalf = (parameters *)malloc(sizeof(parameters));
    firsthalf->start = 0, firsthalf->end = arrSize / 2 - 1;
    parameters *secondhalf = (parameters *)malloc(sizeof(parameters));
    secondhalf->start = arrSize / 2;
    secondhalf->end = arrSize - 1;
    parameters *mergeParame = (parameters *)malloc(sizeof(parameters));
    mergeParame->start = 0;

    /* create the thread */
    pthread_create(&tid, &attr, sortHalf, firsthalf);
    pthread_create(&tid, &attr, sortHalf, secondhalf);

    /* now wait for the two threads to exit */
    pthread_join(tid, NULL);
    pthread_join(tid, NULL);

    pthread_create(&tid, &attr, myMergeSort, mergeParame);
    pthread_join(tid, NULL);
    for (int i = 0; i < arrSize; i++)
    {
        printf("%d ", num[i]);
    }

    return 0;
}

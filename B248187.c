// PPLS Exercise 1 Starter File
//
// See the exercise sheet for details
//
// Note that NITEMS, NTHREADS and SHOWDATA should
// be defined at compile time with -D options to gcc.
// They are the array length to use, number of threads to use
// and whether or not to printout array contents (which is
// useful for debugging, but not a good idea for large arrays).

/* -------------------- Discussion -------------------- */
/* This is just a big picture of the programming. Details can be seen in each comment.

This code implements parallel prefix sum computation using pthreads library in C. 
The parallelprefixsum function divides the array into chunks and assigns each chunk to a separate thread for parallel computation. 
The computation consists of three phases:
1. Each thread computes partial prefix sums for its assigned chunk concurrently.
2. After all threads complete phase 1, thread 0 performs a sequential computation to aggregate the last elements of each chunk.
3. All threads, except for thread 0, collectively compute the prefix sum for the remaining elements in parallel.

For tasks in phase 1 and 3 are quite simple, threads work individually. The only thing to care its passing in correct parameters. 
However, we have to make sure all threads are synchronized between phase 1 and 2, and between phase 2 and 3, to get the correct sum.
The concept of monitor was implemented to realize the synchronization we talked above.
Some points may be focused on:
  1. A counter to count how many threads finished task 1.
  2. Mutex and condition variables are used to realize monitors.
  3. Two condition variables for thread 0 and other threads to make sure phase two is ready to go, because we only want thread 0 to do task 2.
  4. We can view phase 2 as a critical section, which only has thread 0 execute within this section, others just wait thread 0 to finish.
*/



#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // in case you want semaphores, but you don't really
                       // need them for this exercise

// Global data;
int chunkSize;
int lastChunkSize;
int* lastIdxArr;

// Pthreads Monitor (Mostly for synchronization between phase 1 and 2)
int count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condThreadZero = PTHREAD_COND_INITIALIZER;   // For thread 0. 
pthread_cond_t condOtherThread = PTHREAD_COND_INITIALIZER;  // For other threads.

// typedef for a pack of arguments (in this case, three integers)
typedef struct arg_pack {
  int id;
  int *array;     // Pointer to data. 
  int l;          // the first index of chunk
  int r;          // the last idx of chunk
  int prefixSum;  // Store the sum of prefix of previous chunk.
}arg_pack;

// Print a helpful message followed by the contents of an array
// Controlled by the value of SHOWDATA, which should be defined
// at compile time. Useful for debugging.
void showdata(char *message, int *data, int n)
{
  int i;

  if (SHOWDATA)
  {
    printf("%s", message);
    for (i = 0; i < n; i++)
    {
      printf(" %d", data[i]);
    }
    printf("\n");
  }
}

// Check that the contents of two integer arrays of the same length are equal
// and return a C-style boolean
int checkresult(int *correctresult, int *data, int n)
{
  int i;

  for (i = 0; i < n; i++)
  {
    if (data[i] != correctresult[i])
      return 0;
  }
  return 1;
}

// Compute the prefix sum of an array **in place** sequentially
void sequentialprefixsum(int *data, int n)
{
  int i;

  for (i = 1; i < n; i++)
  {
    data[i] = data[i] + data[i - 1];
  }
}

// YOU MUST WRITE THIS FUNCTION AND ANY ADDITIONAL FUNCTIONS YOU NEED

// Thread functions.
void *parallelPrefixSum(void *args){

  // Fetch input data and arguments.
  int id = ((arg_pack *)args)->id;
  int l = ((arg_pack *)args)->l, r = ((arg_pack *)args)->r;
  int *arr = ((arg_pack *)args)->array;

  /*
    Phase 1:
    Deal with each chunk individually and parallelly.
  */
  for(int i = l + 1; i <= r; i++){
    arr[i] = arr[i - 1] + arr[i];
  }
  /* End of phase 1. */

  /* Implement a monitor to synchronize all threads. */ 
  pthread_mutex_lock(&mutex);
  count++;          // Count how many threads finished task 1.
  if(id != 0){      // Other threads
    /*
      Wait for all threads finish task 1.
      Signal thread 0 to do task 2, if this is the last thread in phase 1.
    */
    if(count == NTHREADS) pthread_cond_signal(&condThreadZero); 
    pthread_cond_wait(&condOtherThread, &mutex);
  }
  else{             // Thread 0
    /*
      Wait for all threads finish task 1.
    */
    if(count < NTHREADS) pthread_cond_wait(&condThreadZero, &mutex);
    /*
      Phase 2.
      Thread 0 goes into phase 2 alone sequentially.
      Here, lastIdxArr is an global array storing the last indices of chunks.
    */
    for(int i = 1; i < NTHREADS; i++){
      arr[lastIdxArr[i]] += arr[lastIdxArr[i - 1]];
    }
    /*
      End of phase 2
      -> Wake up other threads to do task 3.
    */
    if(count == NTHREADS) pthread_cond_broadcast(&condOtherThread); 
  }
  /*
    Leaving critical section.
  */
  pthread_mutex_unlock(&mutex);

  /*
    Phase 3.
    Threads go into phase 3 except for thread 0.
  */
  if(id != 0){
    int prefix = arr[l - 1];
    for(int j = l; j < r; j++){
      arr[j] += prefix;
    }
  }
  return NULL;
}

void parallelprefixsum(int *data, int n)
{
  // *** TODO ***

  // Get correct chunk sizes.
  chunkSize = NITEMS / NTHREADS;
  lastChunkSize = NITEMS - (NTHREADS - 1) * chunkSize;
  lastIdxArr = (int *) malloc((NTHREADS * sizeof(int))); 

  // Prepare threads args
  pthread_t  *threads;
  arg_pack   *threadargs;
  int p = NTHREADS; // we will create NTHREADS threads
  
  // Set up Pthreads
  threads     = (pthread_t *) malloc (p * sizeof(pthread_t));
  // Set up handles and argument packs for the worker threads
  threadargs  = (arg_pack *)  malloc (p * sizeof(arg_pack));
  for(int i = 0; i <= NTHREADS - 2; i++){
    threadargs[i].id = i;
    threadargs[i].array = data;
    threadargs[i].l = 0 + i * chunkSize;
    threadargs[i].r = (1 + i) * chunkSize - 1;
    lastIdxArr[i] = threadargs[i].r;
  }
  threadargs[NTHREADS - 1].id = NTHREADS - 1;
  threadargs[NTHREADS - 1].array = data;
  threadargs[NTHREADS - 1].l = NITEMS - lastChunkSize;
  threadargs[NTHREADS - 1].r = NITEMS - 1;
  lastIdxArr[NTHREADS - 1] = NITEMS - 1;

  // Test correct chunk sizes.
  // for(int i = 0; i < NTHREADS; i++){
  //   printf("Chunk %d: l = %d and r = %d\n", i, threadargs[i].l, threadargs[i].r);
  //   printf("Last Index: %d.\n", lastIdxArr[i]);
  // }

  /*
    Execute paraller prefix sum.
  */
  for (int i = 0; i < p; i++) {
    pthread_create(&threads[i], NULL, parallelPrefixSum, (void *) &threadargs[i]);
  }
  // Wait until all threads finish
  for(int i = 0; i < p; i++){
    pthread_join(threads[i], NULL);
  }

  return;
}

int main(int argc, char *argv[])
{

  int *arr1, *arr2, i;

  // Check that the compile time constants are sensible
  if ((NITEMS > 10000000) || (NTHREADS > 32))
  {
    printf("So much data or so many threads may not be a good idea! .... exiting\n");
    exit(EXIT_FAILURE);
  }

  // Create two copies of some random data
  arr1 = (int *)malloc(NITEMS * sizeof(int));
  arr2 = (int *)malloc(NITEMS * sizeof(int));
  srand((int)time(NULL));
  for (i = 0; i < NITEMS; i++)
  {
    arr1[i] = arr2[i] = rand() % 5;
  }
  showdata("initial data          : ", arr1, NITEMS);

  // Calculate prefix sum sequentially, to check against later on
  sequentialprefixsum(arr1, NITEMS);
  showdata("sequential prefix sum : ", arr1, NITEMS);

  // Calculate prefix sum in parallel on the other copy of the original data
  parallelprefixsum(arr2, NITEMS);
  showdata("parallel prefix sum   : ", arr2, NITEMS);

  // Check that the sequential and parallel results match
  if (checkresult(arr1, arr2, NITEMS))
  {
    printf("Well done, the sequential and parallel prefix sum arrays match.\n");
  }
  else
  {
    printf("Error: The sequential and parallel prefix sum arrays don't match.\n");
  }

  free(arr1);
  free(arr2);
  return 0;
}

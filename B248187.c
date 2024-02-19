// PPLS Exercise 1 Starter File
//
// See the exercise sheet for details
//
// Note that NITEMS, NTHREADS and SHOWDATA should
// be defined at compile time with -D options to gcc.
// They are the array length to use, number of threads to use
// and whether or not to printout array contents (which is
// useful for debugging, but not a good idea for large arrays).

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h> // in case you want semaphores, but you don't really
                       // need them for this exercise

// Global data;
int chunkSize;
int lastChunkSize;

// typedef for a pack of arguments (in this case, three integers)
typedef struct arg_pack {
  int *array; // Pointer to data. 
  int l; // first index of chunk
  int r; // last idx ..
  int prefixSum; // Store the sum of prefix of previous chunk.
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
void *prefixSum_phase_one(void *args){
  int l = ((arg_pack *)args)->l, r = ((arg_pack *)args)->r;
  int *arr = ((arg_pack *)args)->array;
  for(int i = l + 1; i <= r; i++){
    arr[i] = arr[i - 1] + arr[i];
  }
  return NULL;
}
void *prefixSum_phase_two(void *args){
  arg_pack *thread_args = ((arg_pack *)args);
  int *arr = thread_args[0].array;
  for(int i = 1; i < NTHREADS; i++){
    arr[thread_args[i].r] += arr[thread_args[i - 1].r];
  }
  return NULL;
}
void *prefixSum_phase_three(void *args){
  int l = ((arg_pack *)args)->l, r = ((arg_pack *)args)->r;
  int *arr = ((arg_pack *)args)->array;
  int prefix = arr[l - 1];
  for(int j = l; j < r; j++){
    arr[j] += prefix;
  }
  return NULL;
}

void parallelprefixsum(int *data, int n)
{
  // ***TODO***
  chunkSize = NITEMS / NTHREADS;
  lastChunkSize = NITEMS - (NTHREADS - 1) * chunkSize;

  // Prepare threads args
  pthread_t  *threads;
  arg_pack   *threadargs;
  int p = NTHREADS; // we will create 8 threads
  
  // Set up Pthreads
  threads     = (pthread_t *) malloc (p * sizeof(pthread_t));
  // Set up handles and argument packs for the worker threads
  threadargs  = (arg_pack *)  malloc (p * sizeof(arg_pack));
  for(int i = 0; i <= NTHREADS - 2; i++){
    threadargs[i].array = data;
    threadargs[i].l = 0 + i * chunkSize;
    threadargs[i].r = (1 + i) * chunkSize - 1;
  }
  threadargs[NTHREADS - 1].array = data;
  threadargs[NTHREADS - 1].l = NITEMS - lastChunkSize;
  threadargs[NTHREADS - 1].r = NITEMS - 1;

  // for(int i = 0; i < NTHREADS; i++){
  //   printf("Chunk %d: l = %d and r = %d\n", i, threadargs[i].l, threadargs[i].r);
  // }


  // Phase 1
  // Create N threads
  // Pass functions into threads
  for (int i = 0; i < p; i++) {
    pthread_create(&threads[i], NULL, prefixSum_phase_one, (void *) &threadargs[i]);
  }
  // Wait until all threads finish
  for(int i = 0; i < p; i++){
    pthread_join(threads[i], NULL);
  }

  // Phase 2
  // Thread 0 works sequentially
  pthread_create(&threads[0], NULL, prefixSum_phase_two, (void *) threadargs);
  pthread_join(threads[0], NULL);

  // Phase 3
  // All threads work individually
  for (int i = 0; i < p; i++) {
    pthread_create(&threads[i], NULL, prefixSum_phase_three, (void *) &threadargs[i]);
  }
  // Wait untill all converges.
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

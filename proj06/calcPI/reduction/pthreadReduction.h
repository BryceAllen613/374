/* pthreadReduction.h is my personal algorithm for 
 * performing reduction on multithreaded programs in hopes
 * of speeding up the traditional mutex algorithm.
 * 
 * Author: Bryce Allen
 * Date: 10/31/2019 
 */


unsigned count = 0;
pthread_cond_t allThreadsPresent = PTHREAD_COND_INITIALIZER;

/* pthreadReductionSum reduces values from multiple threads
 * and sums them into one shared global variable
 * 
 * @param: localVar, the local value from all threads
 * @param: globalVar, the global sum available to all threads
 * @param: reduceArray, the buffer where each thread places localVar
 * @param: numThreads, number of threads being reduced
 * @param: threadID, integer used for indexing reduceArray 
 */
void pthreadReductionSum(long double localVar, 
    long double* globalVar, 
    long double* reduceArray, int numThreads, int threadID)
{
    reduceArray[threadID] = localVar;
    count++;
    if(count == numThreads)
    {
        count = 0;
        //pthread_cond_broadcast( &allThreadsPresent );
        for(int i = 0; i < numThreads; i++)
        {
            *globalVar += reduceArray[i];
        }
    }
}
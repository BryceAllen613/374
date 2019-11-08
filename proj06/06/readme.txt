 To support the Mutual Exclusion pattern, the pthread library provides:

    The pthread_mutex_t type, which can be used to create "lock" variables.
    The pthread_mutex_lock() function, which a thread can use to lock a "lock variable". Any threads that perform this function when the lock is locked will block.
    The pthread_mutex_unlock() function, which can be used to unlock a "lock variable". If there are blocked threads waiting at the lock, exactly one of them will be unblocked and allowed to proceed. 

Taken from https://cs.calvin.edu/courses/cs/374/exercises/06/lab/ . A page managed by Professor Adams, Calvin University.
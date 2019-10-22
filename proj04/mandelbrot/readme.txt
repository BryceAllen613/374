Slices and Chunks are working correctly, mw (master worker) does not work, and outputs this...

[gold00:6084] *** An error occurred in MPI_Send
[gold00:6084] *** reported by process [1169620993,0]
[gold00:6084] *** on communicator MPI_COMM_WORLD
[gold00:6084] *** MPI_ERR_RANK: invalid rank
[gold00:6084] *** MPI_ERRORS_ARE_FATAL (processes in this communicator will now abort,
[gold00:6084] ***    and potentially your MPI job)
--------------------------------------------------------------------------
An MPI communication peer process has unexpectedly disconnected.  This
usually indicates a failure in the peer process (e.g., a crash or
otherwise exiting without calling MPI_FINALIZE first).

Although this local MPI process will likely now behave unpredictably
(it may even hang or crash), the root cause of this problem is the
failure of the peer -- that is what you need to investigate.  For
example, there may be a core file that you can examine.  More
generally: such peer hangups are frequently caused by application bugs
or other external events.

  Local host: gold03
  Local PID:  17467
  Peer host:  gold00
--------------------------------------------------------------------------

... when compiling with the included Makefile and executing with the command 
mpirun -np 4 -oversubscribe -machinefile hosts ./mandelMW


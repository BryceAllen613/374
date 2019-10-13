CC      = mpicc
CFLAGS  = -DMPE_GRAPHICS -c -Wall
LFLAGS  = -o mandelChunks -L/usr/X11R6/lib -lmpe -lX11 -lm

mandelChunks: mandelChunks.o display.o
	$(CC) mandelChunks.o display.o $(LFLAGS)

mandelChunks.o: mandelChunks.c display.h
	$(CC) $(CFLAGS) mandelChunks.c 

display.o: display.c display.h
	$(CC) $(CFLAGS) display.c

run1:
	mpirun -np 4 ./mandelChunks

clean:
	rm -f mandelChunks a.out *.o *~ *# PI*


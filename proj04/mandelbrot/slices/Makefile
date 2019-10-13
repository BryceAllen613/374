CC      = mpicc
CFLAGS  = -DMPE_GRAPHICS -c -Wall
LFLAGS  = -o mandelSlices -L/usr/X11R6/lib -lmpe -lX11 -lm

mandelSlices: mandelSlices.o display.o
	$(CC) mandelSlices.o display.o $(LFLAGS)

mandelSlices.o: mandelSlices.c display.h
	$(CC) $(CFLAGS) mandelSlices.c 

display.o: display.c display.h
	$(CC) $(CFLAGS) display.c

run1:
	mpirun -np 4 ./mandelSlices

clean:
	rm -f mandelSlices a.out *.o *~ *# PI*


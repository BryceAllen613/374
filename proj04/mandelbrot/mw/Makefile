CC      = mpicc
CFLAGS  = -DMPE_GRAPHICS -c -Wall
LFLAGS  = -o mandelMW -L/usr/X11R6/lib -lmpe -lX11 -lm

mandelMW: mandelMW.o display.o
	$(CC) mandelMW.o display.o $(LFLAGS)

mandelMW.o: mandelMW.c display.h
	$(CC) $(CFLAGS) mandelMW.c 

display.o: display.c display.h
	$(CC) $(CFLAGS) display.c

run1:
	mpirun -np 4 ./mandelMW

clean:
	rm -f mandelMW a.out *.o *~ *# PI*


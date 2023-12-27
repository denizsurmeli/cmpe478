CC=g++

all: mpi thrust serial
mpi:
	mpicc -O3 -o build/mpi_part mpi_part.c -lm
thrust:
	$(CC) -O3 thrust_part.c -fopenmp -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_OMP -lm -o build/thrust_part
serial:
	$(CC) -O3 serial.c -o build/serial

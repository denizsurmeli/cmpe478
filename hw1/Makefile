CC=gcc

all: parallel nolock serial
parallel:
	$(CC) -fopenmp parallel.c -o build/parallel -lm
nolock:
	$(CC) -fopenmp nolock.c -o build/nolock -lm
serial:
	$(CC) serial.c -o build/serial

CC=gcc
CFLAGS=-Wall

all: fifo_w fifo_r

fifo_w: fifo_w.c
	$(CC) $(CFLAGS) fifo_w.c -o fifo_w

fifo_r: fifo_r.c
	$(CC) $(CFLAGS) fifo_r.c -o fifo_r

clean:
	rm -f fifo_w fifo_r
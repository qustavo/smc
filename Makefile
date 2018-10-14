CC=gcc
CFLAGS=-Wall -g -O2
SMC=-DSMC
OBJS=mem_demo_smc.o mem_demo.o

all: $(OBJS)

mem_demo_smc.o: mem_demo.c
	$(CC) -c mem_demo.c -o mem_demo_smc.o $(SMC)
	$(CC) $(CFLAGS) mem_demo_smc.o -o mem_demo_smc

mem_demo.o: mem_demo.c
	$(CC) -c mem_demo.c -o mem_demo.o
	$(CC) $(CFLAGS) mem_demo.o -o mem_demo

clean:
	rm -rf $(OBJS) mem_demo mem_demo_smc

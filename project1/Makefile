CC=gcc
exe=main.out
obj=main.o vscheduler.o queue.o

.PHONY:clean

all:$(obj)
	$(CC) -o $(exe) $(obj)
main.o:main.c vscheduler.h
	$(CC) -c main.c 
vscheduler.o:vscheduler.h vscheduler.c queue.h
	$(CC) -c vscheduler.c
queue.o:queue.c
	$(CC) -c queue.c

clean:
	rm -f *.o main.out


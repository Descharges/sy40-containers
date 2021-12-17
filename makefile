OBJS	= main.o transport.o transportGeneration.o
SOURCE	= main.c transport.c transportGeneration.c
HEADER	= main.h transport.h transportGeneration.h container.h
OUT	= sy40
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lpthread

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)


main.o: main.c
	$(CC) $(FLAGS) main.c

transport.o: transport.c
	$(CC) $(FLAGS) transport.c


clean: all
	rm -f $(OBJS)

clear:
	rm -f $(OBJS) $(OUT)

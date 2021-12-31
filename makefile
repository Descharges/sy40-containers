OBJS	= main.o docks.o transport.o crane.o
SOURCE	= main.c docks.c transport.c crane.c
HEADER	= main.h container.h docks.h transport.h crane.h
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

run: clean
	clear && ./$(OUT)

clean: all
	rm -f $(OBJS)

clear:
	rm -f $(OBJS) $(OUT)

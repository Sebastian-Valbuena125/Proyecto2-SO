CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pthread

TARGET = sentinel-dispatcher

OBJS = main.o queue.o parser.o dispatcher.o master.o worker.o metrics.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
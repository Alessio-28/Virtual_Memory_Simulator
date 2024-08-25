CC = gcc
CFLAGS = -Wall

OBJS = 
HEADERS = 
BINS = 

.phony: all clean

all: $(BINS)

%: %.o
	$(CC) -o $@ $<

%.o: %.c
	$(CC) -c $<

clean:
	rm -rf *.o $(BINS)
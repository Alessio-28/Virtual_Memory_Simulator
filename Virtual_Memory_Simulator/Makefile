CC = gcc
CFLAGS = -O3 -Wall -D_TEST_

SRC = MMU.c swap.c array_list.c VMSS_main.c test.c
OBJS = MMU.o swap.o array_list.o VMSS_test.o
HEADERS = VMSS_constants.h MMU.h swap.h array_list.h test.h
BINS = VMSS_main

.phony: all clean

all: $(BINS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

VMSS_main: $(SRC) $(HEADRES)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -rf $(OBJS) $(BINS) swap.txt

clean_s:
	rm -rf swap.txt
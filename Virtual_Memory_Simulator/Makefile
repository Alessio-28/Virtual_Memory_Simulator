CC = gcc
CFLAGS = -Wall -D_TEST_

SRC = MMU.c swap.c array_list.c VMSS_test.c
OBJS = MMU.o swap.o array_list.o VMSS_test.o
HEADERS = VMSS_constants.h MMU.h swap.h array_list.h
BINS = VMSS_test

.phony: all clean

all: $(BINS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

VMSS_test: $(SRC) $(HEADRES)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -rf $(OBJS) $(BINS) swap.txt

clean_s:
	rm -rf swap.txt
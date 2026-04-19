CC = gcc
CFLAGS = -Wall -Wextra -Werror

.PHONY: run clean

build: src/*.c
	$(CC) $(CFLAGS) -o $@ $^ -lreadline

run: build
	./drsh

clean:
	rm -f drsh

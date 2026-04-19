CC = gcc
CFLAGS = -Wall -Wextra -Werror

.PHONY: run clean

drsh: src/*.c
	$(CC) $(CFLAGS) -o $@ $^ -lreadline

run: drsh
	./drsh

clean:
	rm -f drsh

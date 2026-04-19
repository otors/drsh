CC = gcc
CFLAGS = -Wall -Wextra -Werror

.PHONY: drshell clean

build:
	$(CC) $(CFLAGS) -o drsh src/*.c -lreadline

drsh: build
	./drsh	

clean:
	rm -f drsh

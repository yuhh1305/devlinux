CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
TARGETS = daemon cli

all: $(TARGETS)

daemon: daemon.c sensor_shm.h
	$(CC) $(CFLAGS) -o daemon daemon.c

cli: cli.c sensor_shm.h
	$(CC) $(CFLAGS) -o cli cli.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
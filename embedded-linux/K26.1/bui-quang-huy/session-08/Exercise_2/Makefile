CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
TARGETS = receiver sender

all: $(TARGETS)

receiver: receiver.c
	$(CC) $(CFLAGS) -o receiver receiver.c

sender: sender.c
	$(CC) $(CFLAGS) -o sender sender.c

clean:
	rm -f $(TARGETS)

.PHONY: all clean
CC = gcc
CFLAGS = -Wall -Wextra -pthread
TARGET = booking
SRCS = booking.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
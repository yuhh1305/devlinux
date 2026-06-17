# Makefile cho Exercise_1
CC = gcc
CFLAGS = -Wall -Wextra -O2

TARGET = manager
SOURCES = manager.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

.PHONY: all clean
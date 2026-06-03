CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11

TARGET  = main

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) students.dat

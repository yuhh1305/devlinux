CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11

.PHONY: all test_flock test_fcntl clean

all: writer_flock writer_fcntl

writer_flock: writer_flock.c
	$(CC) $(CFLAGS) -o $@ $<

writer_fcntl: writer_fcntl.c
	$(CC) $(CFLAGS) -o $@ $<

test_flock: writer_flock
	rm -f system.log
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		./writer_flock "message $$i" & \
	done; \
	wait
	@echo "--- flock result ---"
	@wc -l system.log
	@grep -c "^\[PID" system.log

test_fcntl: writer_fcntl
	rm -f system.log
	@for i in 1 2 3 4 5 6 7 8 9 10; do \
		./writer_fcntl "message $$i" & \
	done; \
	wait
	@echo "--- fcntl result ---"
	@wc -l system.log
	@grep -c "^\[PID" system.log

clean:
	rm -f writer_flock writer_fcntl system.log

# Target library

lib := libuthread.a

all: $(lib)

## TODO: Phase 1.1

$(lib): queue.o
	ar rcs $(lib) queue.o

queue.o: queue.c queue.h
	gcc -Wall -Werror -c queue.c

# SHOULD BE REMOVED LATER ON
queue: queue.c queue.h
	gcc -Wall -Werror -o queue queue.c

clean:
	rm -rf *.a *.o

# SHOULD BE REMOVED LATER ON
qclean:
	rm -rf queue

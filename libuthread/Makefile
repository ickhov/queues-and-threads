# Target library

lib := libuthread.a

all: $(lib)

## TODO: Phase 1.1

# $(lib): queue.o uthread context.o preempt.o
# 	ar rcs $(lib) queue.o uthread context.o preempt.o
$(lib): queue.o uthread.o context.o preempt.o
	ar rcs $(lib) queue.o uthread.o context.o preempt.o

queue.o: queue.c queue.h
	gcc -Wall -Werror -c queue.c

# SHOULD BE REMOVED LATER ON
# queue.o: queue.c queue.h
	# gcc -Wall -Werror -c queue.c

context.o: context.c context.h preempt.h uthread.h
	gcc -Wall -Werror -c context.c

preempt.o: preempt.c preempt.h uthread.h
	gcc -Wall -Werror -c preempt.c

# SHOULD BE REMOVED LATER ON
# uthread.o: uthread.c uthread.h queue.o context.o preempt.o
uthread.o: uthread.c uthread.h queue.h context.h preempt.h
	gcc -Wall -Werror -c uthread.c

clean:
	rm -rf *.a *.o

# SHOULD BE REMOVED LATER ON
#qclean:
#	rm -rf queue uthread *.o
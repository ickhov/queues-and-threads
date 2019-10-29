# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o
deps := queue.d uthread.d context.d preempt.d

CC := gcc
CFLAGS := -Wall -Werror
PANDOC := pandoc

ifneq ($(V),1)
Q = @
endif

## Debug flag
ifneq ($(D),1)
CFLAGS	+= -O2
else
CFLAGS	+= -O0
CFLAGS	+= -g
endif

all: $(lib) 

dep := $(patsubst %.o,%.d,$(objs))
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

$(lib): $(objs)
	$(Q) ar rcs $@ $^

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)

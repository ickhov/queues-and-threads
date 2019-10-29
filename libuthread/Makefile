# Target library
lib := libuthread.a
objs := queue.o uthread.o context.o preempt.o
deps := queue.d uthread.d context.d preempt.d

CC := gcc
CFLAGS := -Wall -Werror
CFLAGS += -g
PANDOC := pandoc

ifneq ($(V),1)
Q = @
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

%.html: %.md
	@echo "MD $@"
	$(Q)$(PANDOC) -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)

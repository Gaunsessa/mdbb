PROJ := mdbb

CC := clang

SRCDIR   := src
BUILDDIR := build

SRCS := $(shell find $(SRCDIR) -name '*.c')
OBJS := $(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS))

INCLUDES := -I inc

build-test : $(BUILDDIR)/$(PROJ).a test.c
	$(CC) -o $(BUILDDIR)/test $(INCLUDES) $(BUILDDIR)/$(PROJ).a test.c -fsanitize=address

build : $(BUILDDIR)/$(PROJ).a

$(BUILDDIR)/$(PROJ).a : $(OBJS)
	ar r $(BUILDDIR)/$(PROJ).a $(OBJS)

$(BUILDDIR)/%.o : %.c
	mkdir -p $(dir $@)
	$(CC) -c -o $@ $(INCLUDES) $(patsubst $(BUILDDIR)/%.o,%.c,$@)

clean:
	rm -rf $(BUILDDIR)/*
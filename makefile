CC = gcc
LVGL_DIR = ./lvgl
LIBS_DIR = ./libs
JANSSON_DIR = $(LIBS_DIR)/jansson
CFLAGS = -Wall -Wshadow -Wundef -Wmaybe-uninitialized -O3 -g0 -I$(LVGL_DIR) -I$(JANSSON_DIR) -I$(LIBS_DIR) -std=c99
LDFLAGS = -lm -lSDL2
BINDIR := bin
OBJDIR := obj
BIN = $(BINDIR)/output

MAINSRC = main.c

# Find LVGL source files if they exist, otherwise use empty
CSRCS = $(shell find $(LVGL_DIR)/src -name "*.c" 2>/dev/null || echo "")
# Add this line to recursively find all .c files in src folder:
SRCSRC = $(shell find ./src -name "*.c" 2>/dev/null || echo "")
# Find library source files (exclude jansson folder to avoid duplicates)
LIBSRCS = $(shell find $(LIBS_DIR) -name "*.c" -not -path "$(JANSSON_DIR)/*" 2>/dev/null || echo "")
# Find jansson source files
JANSSON_SRCS = $(shell find $(JANSSON_DIR) -name "*.c" 2>/dev/null || echo "")

ASRCS = 

INC := -I . -I $(LVGL_DIR) -I $(LIBS_DIR) -I $(JANSSON_DIR)

OBJEXT ?= .o

# Create object files in obj directory, preserving directory structure
AOBJS = $(patsubst %.S,$(OBJDIR)/%.o,$(ASRCS))
COBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(CSRCS))
MAINOBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(MAINSRC))
SRCOBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCSRC))
LIBOBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(LIBSRCS))
JANSSON_OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(JANSSON_SRCS))

SRCS = $(ASRCS) $(CSRCS) $(MAINSRC) $(SRCSRC) $(LIBSRCS) $(JANSSON_SRCS)
OBJS = $(AOBJS) $(COBJS) $(SRCOBJS) $(LIBOBJS) $(JANSSON_OBJS)

all: default

# Create object files in obj directory, creating subdirectories as needed
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@
	@echo "CC $< -> $@"

default: $(MAINOBJ) $(AOBJS) $(COBJS) $(SRCOBJS) $(LIBOBJS) $(JANSSON_OBJS)
	@mkdir -p $(BINDIR)
	$(CC) -o $(BIN) $(MAINOBJ) $(AOBJS) $(COBJS) $(SRCOBJS) $(LIBOBJS) $(JANSSON_OBJS) $(LDFLAGS)

run: default
	@echo "Running $(BIN)..."
	@./$(BIN)

clean: 
	rm -rf $(BIN) $(OBJDIR)

.PHONY: all default clean
# Makefile for Toy Json Parser proj:
# By: Derek Tan

# Compiler:
CC := gcc -std=c11
CFLAGS := -Wall -Werror

# Directories
HDR_DIR := ./headers
SRC_DIR := ./src
BIN_DIR := ./bin

# File Selectors
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,%.o,$(SRCS))
EXE := $(BIN_DIR)/myjson

# Directives
vpath %.c $(SRC_DIR)

.PHONY: all listobjs clean

# Rules:
listobjs:
	@echo "Object targets:"
	@echo $(OBJS)

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -I$(HDR_DIR)

clean:
	rm -f $(EXE)

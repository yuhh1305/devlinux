# Toolchain Configurations
CC      := gcc
CFLAGS  := -Wall -Wextra -Iinclude -fPIC
AR      := ar
ARFLAGS := rcs

# Output Directories
LIB_DIR := lib
OBJ_DIR := obj
BIN_DIR := bin

# Output Targets
STATIC_LIB := $(LIB_DIR)/libcalc.a
SHARED_LIB := $(LIB_DIR)/liblogger.so
EXEC       := $(BIN_DIR)/main

# Dynamic Source and Object Discovery 
SRCS       := $(wildcard src/*.c)
OBJS       := $(patsubst src/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Filter individual functional layers out dynamically
MAIN_OBJ   := $(OBJ_DIR)/main.o
CALC_OBJ   := $(OBJ_DIR)/calc.o
LOG_OBJ    := $(OBJ_DIR)/logger.o

# Dynamic Linker Flags (Rpath links execution directly to runtime location)
LDFLAGS    := -L$(LIB_DIR) -lcalc -llogger -Wl,-rpath,'$$ORIGIN/../lib'

# Phony Interfaces
.PHONY: all libs static shared run clean rebuild

# 1. Main Project Target
all: $(EXEC)

# Executable Construction Rule
$(EXEC): $(MAIN_OBJ) $(STATIC_LIB) $(SHARED_LIB) | $(BIN_DIR)
	$(CC) $(MAIN_OBJ) $(LDFLAGS) -o $@

# 2. Layered Component Targets
libs: static shared

static: $(STATIC_LIB)

shared: $(SHARED_LIB)

# Core Library Composition Drivers
$(STATIC_LIB): $(CALC_OBJ) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^

$(SHARED_LIB): $(LOG_OBJ) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

# 3. Generic Pattern Rule for Compilation
$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Dynamic Directory Generation
$(OBJ_DIR) $(LIB_DIR) $(BIN_DIR):
	mkdir -p $@

# 4. Diagnostics & Automation Targets
run: $(EXEC)
	./$(EXEC)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(BIN_DIR) app.log

rebuild: clean all
# Compiler and Flags
CC      := gcc
CFLAGS  := -Wall -Wextra -Iinclude -g
AR      := ar
ARFLAGS := rcs

# Directories
SRC_DIR := src
APP_DIR := app
INC_DIR := include
OBJ_DIR := obj
LIB_DIR := lib

# Outputs
STATIC_LIB := $(LIB_DIR)/libmathutils.a
EXEC       := main

# Source and Object files
LIB_SRCS   := $(SRC_DIR)/mathutils.c
LIB_OBJS   := $(OBJ_DIR)/mathutils.o
APP_SRCS   := $(APP_DIR)/main.c

# Phony Targets
.PHONY: all clean run

# Default Target
all: $(EXEC)

# Rule to link the main executable using the static library
$(EXEC): $(APP_SRCS) $(STATIC_LIB)
	$(CC) $(CFLAGS) $(APP_SRCS) -L$(LIB_DIR) -lmathutils -o $(EXEC)

# Rule to build the static library
$(STATIC_LIB): $(LIB_OBJS) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^

# Rule to compile library object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure output directories exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

# Run Target: Builds if necessary, then launches the executable
run: $(EXEC)
	./$(EXEC)

# Clean Target: Removes all build artifacts
clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(EXEC)
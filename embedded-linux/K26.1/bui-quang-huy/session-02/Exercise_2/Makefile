# Compiler and Flags
CC      := gcc
CFLAGS  := -Wall -Wextra -Iinclude -fPIC
LDFLAGS := -Lbin -lstringutils -Wl,-rpath,'$$ORIGIN'

# Directories
SRC_DIR   := src
APP_DIR   := app
BUILD_DIR := build
BIN_DIR   := bin

# Target Names
SHARED_LIB := $(BIN_DIR)/libstringutils.so
EXEC       := $(BIN_DIR)/main
SAMPLE_ARG := "Hello Linux Shared Library 2026"

# Phony Targets
.PHONY: all clean run

# Default Target
all: $(EXEC)

# Step 3: Link main program with the shared library
$(EXEC): $(APP_DIR)/main.c $(SHARED_LIB)
	$(CC) -Wall -Wextra -Iinclude $(APP_DIR)/main.c $(LDFLAGS) -o $(EXEC)

# Step 2: Build the shared library (.so) from object files
$(SHARED_LIB): $(BUILD_DIR)/stringutils.o | $(BIN_DIR)
	$(CC) -shared -o $@ $^

# Step 1: Compile library source with Position Independent Code flag
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Directory Creation Rules
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Run Target: Passes runtime environment configuration to find the .so file
run: $(EXEC)
	./$(EXEC) $(SAMPLE_ARG)

# Clean Target
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
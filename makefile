# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++20 -O3 -flto -fuse-ld=lld -march=native -fexceptions -fopenmp -mavx2 -Wall -Wextra -DNDEBUG
LDFLAGS :=

# Debug compiler flags
DEBUG_CXXFLAGS := -g -O0 -DDEBUG

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
PGO_DIR := pgo_data

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Binary name (set to Jet)
TARGET := $(BIN_DIR)/Jet

# Append .exe to the binary name on Windows
ifeq ($(OS),Windows_NT)
    TARGET := $(TARGET).exe
endif

# Default target
all: $(TARGET)

# Rule to build the target binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)

# Rule to build object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create directories if they don't exist
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Debug target
debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
debug: $(TARGET)

# Clean the build
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(PGO_DIR)

# PGO-generate target
pgo-generate: CXXFLAGS += -fprofile-generate
pgo-generate: clean $(TARGET)

pgo-pg: CXXFLAGS += -pg
pgo-pg: clean $(TARGET)

# Phony targets
.PHONY: all debug clean pgo-generate

# Disable built-in rules and variables
.SUFFIXES:

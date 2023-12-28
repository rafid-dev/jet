# Compiler and flags
CXX := clang++-17
CXXFLAGS := -std=c++20 -flto -march=native -fexceptions -Wall -Wextra
LDFLAGS :=


# Debug compiler flags
DEBUG_CXXFLAGS := -g3 -O1 -DDEBUG -fsanitize=address -fsanitize=undefined 

BUILD_CXXFLAGS := -DNDEBUG -O3

# Directories
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Binary name (set to Jet)
TARGET := $(BIN_DIR)/Jet

# Append .exe to the binary name on Windows
ifeq ($(OS),Windows_NT)
	CXXFLAGS += -fuse-ld=lld
    TARGET := $(TARGET).exe
endif

# Default target
all: CXXFLAGS += $(BUILD_CXXFLAGS)
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

# Phony targets
.PHONY: all debug clean pgo-generate

# Disable built-in rules and variables
.SUFFIXES:
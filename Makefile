# Compiler and Flags
CXX := g++
CXXFLAGS := -ggdb -march=native -DDEBUG
CXXFLAGS_O3 := $(CXXFLAGS) -O3

# Project Structure
ROOT := .
INSTR_LVL_OPT := $(ROOT)/instruction_level_optimization
UTILS := $(ROOT)/utils

# Include Paths
INCLUDES := -I.

# Source and Output Files
SRC := $(INSTR_LVL_OPT)/reduce.cpp
BIN_DIR := $(INSTR_LVL_OPT)/bin
REGULAR_BIN := $(BIN_DIR)/reduce.o
OPTIMIZED_BIN := $(BIN_DIR)/reduce_o3.o



# Build Targets
all: $(BIN_DIR) $(REGULAR_BIN) $(OPTIMIZED_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(REGULAR_BIN): $(SRC) $(UTILS)/*
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(REGULAR_BIN) $(SRC)

$(OPTIMIZED_BIN): $(SRC) $(UTILS)/*
	$(CXX) $(CXXFLAGS_O3) $(INCLUDES) -o $(OPTIMIZED_BIN) $(SRC)

# Run target
run: all
	$(REGULAR_BIN) $(ARGS)

# Run target O3
run_o3: all
	$(OPTIMIZED_BIN) $(ARGS)

# Clean Target
clean:
	rm -rf $(BIN_DIR)

.PHONY: all run_o3 clean
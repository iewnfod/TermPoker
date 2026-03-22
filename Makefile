# Define variables
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -I include
TARGET = termpoker
BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

# Find all source files in the src directory
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)

# Generate object file names in the build directory
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))

# Default goal: build the target executable
all: $(BIN_DIR)/$(TARGET)

# Rule to create the final executable from object files
$(BIN_DIR)/$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@

# Rule to create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Pattern rule to compile .cpp files into .o files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Phony targets
.PHONY: all clean run

# Clean rule: remove build artifacts
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Run rule: build and execute the program
run: all
	./$(BIN_DIR)/$(TARGET)

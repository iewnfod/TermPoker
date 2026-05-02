# Check OS
ifeq ($(OS),Windows_NT)
    # Windows
    CXXFLAGS_EXTRA = -D_WIN32_WINNT=0x0600
    LIBS = -lshell32
    RM = del /Q
    MKDIR = mkdir
else
    # Unix/Linux/macOS
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        # Linux
        CXXFLAGS_EXTRA =
        LIBS =
    endif
    ifeq ($(UNAME_S),Darwin)
        # macOS
        CXXFLAGS_EXTRA =
        LIBS =
    endif
    RM = rm -rf
    MKDIR = mkdir -p
endif

# VARS
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -I include -I ext-lib $(CXXFLAGS_EXTRA)
TARGET = termpoker
BUILD_DIR = build
BIN_DIR = bin
SRC_DIR = src

# Files
SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.d,$(SOURCES))

# Default Target
all: $(BIN_DIR)/$(TARGET)

# Link
$(BIN_DIR)/$(TARGET): $(OBJECTS) | $(BIN_DIR)
	$(CXX) $(OBJECTS) -o $@ $(LIBS)

# Build Dir
$(BUILD_DIR):
	$(MKDIR) $(BUILD_DIR)

# Bin Dir
$(BIN_DIR):
	$(MKDIR) $(BIN_DIR)

# Build
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Include depend
-include $(DEPENDS)

# Clean
clean:
	$(RM) $(BUILD_DIR) $(BIN_DIR)

# Run
run: all
	$(BIN_DIR)/$(TARGET)

.PHONY: all clean run

# Compiler and flags
CXX = g++
CXXFLAGS = -I./hs -Wall -Wextra -std=c++17

# Folder structure
OBJ_DIR = ./os
INC_DIR = ./hs

# Main source file in the root folder
MAIN_SRC = main.cpp

# Main object
MAIN_OBJ = $(OBJ_DIR)/main.o

# Output binary
TARGET = main

# Default rule
all: $(TARGET)

# Linking the binary
$(TARGET): $(MAIN_OBJ)
	$(CXX) $(MAIN_OBJ) -o $@

# Compiling main.cpp into an .o file (from root folder)
$(OBJ_DIR)/main.o: $(MAIN_SRC)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Cleaning build files
clean:
	rm -rf $(OBJ_DIR)/*.o $(TARGET)

# Phony targets
.PHONY: all clean


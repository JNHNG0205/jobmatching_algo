# Makefile for Job Matching System - Algorithm Comparison
# Compiles two versions with different algorithms

CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra

# Target executables
PROGRAM_LINEAR = program_linear
PROGRAM_BOOLEAN = program_boolean
DATA_CLEANING = data_cleaning
ORIGINAL_PROGRAM = program

# Source files
SRC_LINEAR = main_linear_bubble.cpp
SRC_BOOLEAN = main_boolean_selection.cpp
SRC_DATA_CLEANING = data_cleaning.cpp
SRC_ORIGINAL = main.cpp

# Array implementation files
ARRAY_LINEAR = array_linear_bubble.cpp
ARRAY_BOOLEAN = array_boolean_selection.cpp
ARRAY_ORIGINAL = array.cpp

.PHONY: all clean linear boolean original data_cleaning help run_linear run_boolean test

# Default target - compile all versions
all: linear boolean original data_cleaning
	@echo "=========================================="
	@echo "All programs compiled successfully!"
	@echo "=========================================="
	@echo "Available programs:"
	@echo "  1. $(PROGRAM_LINEAR)  - Linear Search + Bubble Sort"
	@echo "  2. $(PROGRAM_BOOLEAN) - Boolean Search + Selection Sort"
	@echo "  3. $(ORIGINAL_PROGRAM)       - Original implementation"
	@echo "  4. $(DATA_CLEANING)   - Data cleaning utility"
	@echo ""
	@echo "Run 'make help' for usage instructions"
	@echo "=========================================="

# Compile Linear Search + Bubble Sort version
linear: $(PROGRAM_LINEAR)

$(PROGRAM_LINEAR): $(SRC_LINEAR) $(ARRAY_LINEAR)
	@echo "Compiling Linear Search + Bubble Sort version..."
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_LINEAR) $(SRC_LINEAR)
	@echo "✓ $(PROGRAM_LINEAR) compiled successfully!"

# Compile Boolean Search + Selection Sort version
boolean: $(PROGRAM_BOOLEAN)

$(PROGRAM_BOOLEAN): $(SRC_BOOLEAN) $(ARRAY_BOOLEAN)
	@echo "Compiling Boolean Search + Selection Sort version..."
	$(CXX) $(CXXFLAGS) -o $(PROGRAM_BOOLEAN) $(SRC_BOOLEAN)
	@echo "✓ $(PROGRAM_BOOLEAN) compiled successfully!"

# Compile original version
original: $(ORIGINAL_PROGRAM)

$(ORIGINAL_PROGRAM): $(SRC_ORIGINAL) $(ARRAY_ORIGINAL)
	@echo "Compiling original version..."
	$(CXX) $(CXXFLAGS) -o $(ORIGINAL_PROGRAM) $(SRC_ORIGINAL)
	@echo "✓ $(ORIGINAL_PROGRAM) compiled successfully!"

# Compile data cleaning utility
data_cleaning: $(DATA_CLEANING)

$(DATA_CLEANING): $(SRC_DATA_CLEANING)
	@echo "Compiling data cleaning utility..."
	$(CXX) $(CXXFLAGS) -o $(DATA_CLEANING) $(SRC_DATA_CLEANING)
	@echo "✓ $(DATA_CLEANING) compiled successfully!"

# Run Linear Search + Bubble Sort version
run_linear: linear
	@echo "=========================================="
	@echo "Running Linear Search + Bubble Sort version"
	@echo "=========================================="
	./$(PROGRAM_LINEAR)

# Run Boolean Search + Selection Sort version
run_boolean: boolean
	@echo "=========================================="
	@echo "Running Boolean Search + Selection Sort version"
	@echo "=========================================="
	./$(PROGRAM_BOOLEAN)

# Run original version
run_original: original
	@echo "=========================================="
	@echo "Running Original version"
	@echo "=========================================="
	./$(ORIGINAL_PROGRAM)

# Test both versions with sample data
test: linear boolean
	@echo "=========================================="
	@echo "Testing both algorithm versions"
	@echo "=========================================="
	@echo ""
	@echo "Note: This requires manual interaction."
	@echo "Run each program separately to test:"
	@echo "  make run_linear"
	@echo "  make run_boolean"

# Clean compiled files
clean:
	@echo "Cleaning compiled files..."
	rm -f $(PROGRAM_LINEAR) $(PROGRAM_BOOLEAN) $(ORIGINAL_PROGRAM) $(DATA_CLEANING)
	rm -f *.exe
	@echo "✓ Clean complete!"

# Show help information
help:
	@echo "=========================================="
	@echo "Job Matching System - Algorithm Comparison"
	@echo "=========================================="
	@echo ""
	@echo "Available make targets:"
	@echo ""
	@echo "  make all          - Compile all versions (default)"
	@echo "  make linear       - Compile Linear Search + Bubble Sort version"
	@echo "  make boolean      - Compile Boolean Search + Selection Sort version"
	@echo "  make original     - Compile original version"
	@echo "  make data_cleaning - Compile data cleaning utility"
	@echo ""
	@echo "  make run_linear   - Compile and run Linear version"
	@echo "  make run_boolean  - Compile and run Boolean version"
	@echo "  make run_original - Compile and run Original version"
	@echo ""
	@echo "  make clean        - Remove all compiled files"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "=========================================="
	@echo "Algorithm Comparison:"
	@echo "=========================================="
	@echo ""
	@echo "Version 1: Linear Search + Bubble Sort"
	@echo "  - Simple, no preprocessing"
	@echo "  - O(n) search, O(n²) sort"
	@echo "  - Best for small datasets"
	@echo ""
	@echo "Version 2: Boolean Search + Selection Sort"
	@echo "  - Uses inverted index"
	@echo "  - O(1) search (avg), O(n²) sort"
	@echo "  - Best for large datasets, repeated searches"
	@echo ""
	@echo "See ALGORITHM_COMPARISON.md for detailed analysis"
	@echo "=========================================="

# Rebuild everything from scratch
rebuild: clean all


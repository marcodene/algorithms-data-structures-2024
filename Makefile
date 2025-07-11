# Pastry Management System Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Werror -std=gnu11 -O2
LDFLAGS = -lm

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_DIR = tests/public

# Source files
SOURCES = $(SRC_DIR)/main.c
TARGET = pasticceria

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

# Debug build
debug: CFLAGS += -g3 -DDEBUG
debug: $(TARGET)

# Address sanitizer build
asan: CFLAGS += -fsanitize=address -g3
asan: $(TARGET)

# Clean build artifacts
clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f core
	rm -f vgcore.*
	rm -f temp_output.txt
	rm -f actual_output.txt
	rm -f output.txt

# Test targets
test: $(TARGET)
	@echo "Running example test..."
	./$(TARGET) < $(TEST_DIR)/example.txt

# Test a specific file: make test-file FILE=example
test-file: $(TARGET)
	@if [ -z "$(FILE)" ]; then \
		echo "Usage: make test-file FILE=testname (without .txt)"; \
		echo "Example: make test-file FILE=example"; \
		exit 1; \
	fi
	@echo "Testing $(FILE)..."
	@./$(TARGET) < $(TEST_DIR)/$(FILE).txt > temp_output.txt
	@if diff -q temp_output.txt $(TEST_DIR)/$(FILE).output.txt > /dev/null 2>&1; then \
		echo "✓ $(FILE) PASSED"; \
	else \
		echo "✗ $(FILE) FAILED"; \
		echo "Differences:"; \
		diff $(TEST_DIR)/$(FILE).output.txt temp_output.txt; \
	fi
	@rm -f temp_output.txt

test-all: $(TARGET)
	@echo "Running all public tests..."
	@for test in $(TEST_DIR)/*.txt; do \
		if [[ $$test != *.output.txt ]]; then \
			echo "Testing $$(basename $$test)..."; \
			./$(TARGET) < $$test > temp_output.txt; \
			expected=$${test%.txt}.output.txt; \
			if diff -q temp_output.txt $$expected > /dev/null 2>&1; then \
				echo "✓ $$(basename $$test) PASSED"; \
			else \
				echo "✗ $$(basename $$test) FAILED"; \
				echo "  Expected: $$expected"; \
				echo "  Got different output"; \
			fi; \
		fi; \
	done
	@rm -f temp_output.txt

# Memory leak check
memcheck: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) < $(TEST_DIR)/example.txt

# Profiling
profile: $(TARGET)
	valgrind --tool=callgrind ./$(TARGET) < $(TEST_DIR)/example.txt

# Help
help:
	@echo "Available targets:"
	@echo "  all       - Build the project (default)"
	@echo "  debug     - Build with debug information"
	@echo "  asan      - Build with address sanitizer"
	@echo "  clean     - Remove build artifacts"
	@echo "  test      - Run example test case"
	@echo "  test-all  - Run all public test cases"
	@echo "  memcheck  - Run with valgrind memory checker"
	@echo "  profile   - Run with valgrind profiler"
	@echo "  help      - Show this help message"

.PHONY: all debug asan clean test test-all memcheck profile help
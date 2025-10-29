CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c11
LDFLAGS = -lm

# Target executable
TARGET = linkedin_queens_solution

# Source files
SRCS = main.c puzzle.c region.c solver.c validator.c display.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files
HEADERS = puzzle.h region.h solver.h validator.h display.h

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete!"

# Compile source files to object files
%.o: %.c $(HEADERS)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(OBJS) $(TARGET)
	@echo "Clean complete!"

# Run with default 8x8 board
run: $(TARGET)
	./$(TARGET) 8

# Run with regions displayed
run-regions: $(TARGET)
	./$(TARGET) 8 --regions

# Run finding all solutions
run-all: $(TARGET)
	./$(TARGET) 8 --all --regions

.PHONY: all clean rebuild run run-regions run-all
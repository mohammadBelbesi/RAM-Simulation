# Specify the compiler to use
CC = g++

# Compiler flags
CFLAGS = -Wall -Wextra

# Target executable
TARGET = app

# Source files
SRCS = main.cpp sim_mem.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Compile source files into object files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)


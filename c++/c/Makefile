# the compiler to use
CC = clang

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall -emit-llvm
  
#files to link:
LFLAGS = -lcs50
  
# the name to use for both the target source file, and the output file:
TARGET = hello
  
all: $(TARGET)
  
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET).bc -c $(TARGET).c
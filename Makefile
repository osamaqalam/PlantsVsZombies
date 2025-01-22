CC = gcc 							# specifies the compiler to use
CFLAGS = -Iinclude -g				# specifies compiler options
SRC = $(wildcard src/*.c)
TEST_SRC = $(wildcard tests/*.c)
UNITY_SRC = tests/unity/unity.c
TARGET = build/app				    # specifies the output file
TEST_TARGET = build/run_tests	    # specifies the output file for tests

all: $(TARGET)

# Creates the build/ directory if it doesn’t exist
# $@ represents the $(TARGET) ,$^ represent $(SRC)
$(TARGET): $(SRC)					
	mkdir -p build				
	$(CC) $(CFLAGS) -o $@ $^

test: $(TEST_SRC)
	mkdir -p build				
	$(CC) $(CFLAGS) -o $(TEST_TARGET) $(TEST_SRC) $(UNITY_SRC) $(filter-out src/main.c, $(SRC))
	./$(TEST_TARGET)

clean:
	rm -rf $(TARGET) $(TEST_TARGET)

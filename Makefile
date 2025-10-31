CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
TARGET = spell
TESTER = tests

all: $(TARGET) $(TESTER)

$(TARGET): spell.c
	$(CC) $(CFLAGS) -o $(TARGET) spell.c

$(TESTER): tests.c
	$(CC) $(CFLAGS) -o $(TESTER) tests.c

# Run the tests
test: all
	@echo "Running automated tests..."
	@./$(TESTER)

clean:
	rm -f $(TARGET) $(TESTER) output.txt


CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
TARGET = spell
TESTER = run_tests

all: $(TARGET) $(TESTER)

$(TARGET): spell.c
	$(CC) $(CFLAGS) -o $(TARGET) spell.c

$(TESTER): run_tests.c
	$(CC) $(CFLAGS) -o $(TESTER) run_tests.c

# Run the tests
test: all
	@echo "Running automated tests..."
	@./$(TESTER)

clean:
	rm -f $(TARGET) $(TESTER) output.txt

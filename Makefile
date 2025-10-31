CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2

#Executables
SPELL = spell
TESTER = tests

#Default target
all: $(SPELL) $(TESTER)

#Build spell program
$(SPELL): spell.c
	$(CC) $(CFLAGS) -o $(SPELL) spell.c

#Build test runner
$(TESTER): tests.c
	$(CC) $(CFLAGS) -o $(TESTER) tests.c

#Run automated tests
test: all
	@echo "Running Tests..."
	@./$(TESTER)

#Clean build output
clean:
	rm -f $(SPELL) $(TESTER) output.txt

.PHONY: all test clean

CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
TARGET = spell

all: $(TARGET)

$(TARGET): spell.c
	$(CC) $(CFLAGS) -o $(TARGET) spell.c

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean

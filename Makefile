CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
TARGET = train
SRC = main.c functions.c

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

test: $(TARGET)
	./checker.sh ./$(TARGET)

clean:
	rm -f $(TARGET)

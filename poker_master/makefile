CC = gcc

CFLAGS = -Wall -Wextra -std=c11 -Iinclude

TARGET = poker

SRC = src/main.c \
      src/card.c \
      src/deck.c \
      src/poker.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -f $(TARGET)
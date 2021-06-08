# I pledge my honor that I have abided by the Stevens Honor System.
# Jose de la Cruz
# Breanna Shinn

CC     = gcc
C_FILE = $(wildcard *.c)
TARGET = $(patsubst %.c,%,$(C_FILE))
CFLAGS = -g -Wall -Werror -pedantic-errors -pthread

all:
	$(CC) $(CFLAGS) $(C_FILE) -o $(TARGET) -lm
clean:
	rm -f $(TARGET) $(TARGET).exe

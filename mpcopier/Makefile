CC = gcc
CFLAGS = -std=c11 -Wall -Werror
LDFLAGS = -lpthread
TARGET = mpcopier

all: $(TARGET)

$(TARGET): mpcopier.c
	$(CC) $(CFLAGS) -o $(TARGET) mpcopier.c $(LDFLAGS)

clean:
	rm -f $(TARGET)

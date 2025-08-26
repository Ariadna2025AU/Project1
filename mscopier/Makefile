CC = gcc
CFLAGS = -std=c11 -Wall -Werror
LDFLAGS = -lpthread
TARGET = mscopier

all: $(TARGET)

$(TARGET): mscopier.c
	$(CC) $(CFLAGS) -o $(TARGET) mscopier.c $(LDFLAGS)

clean:
	rm -f $(TARGET)

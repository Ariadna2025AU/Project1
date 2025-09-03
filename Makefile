CC = gcc
CFLAGS = -std=c11 -Wall -Werror
LDFLAGS = -lpthread

# Targets
TARGETS = mmcopier mscopier

all: $(TARGETS)

mmcopier: mmcopier.c
	$(CC) $(CFLAGS) -o mmcopier mmcopier.c $(LDFLAGS)

mscopier: mscopier.c
	$(CC) $(CFLAGS) -o mscopier mscopier.c $(LDFLAGS)

clean:
	rm -f $(TARGETS)

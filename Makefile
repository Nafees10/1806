CC = gcc
CFLAGS = -Wall -Wextra -static
TARGET = 1806

RELEASE_CFLAGS = -O2 -DNDEBUG
DEBUG_CFLAGS = -O0 -g

all: release

release: $(TARGET)

debug: clean
	$(CC) $(CFLAGS) $(DEBUG_CFLAGS) -o $(TARGET) 1806.c

$(TARGET): 1806.c
	$(CC) $(CFLAGS) $(RELEASE_CFLAGS) -o $(TARGET) 1806.c

clean:
	rm -f $(TARGET)

CC = gcc
CFLAGS = -O3 -Wall
LDFLAGS = -pthread
TARGET = array_stats

all: $(TARGET)

$(TARGET): array_stats.c
	$(CC) $(CFLAGS) array_stats.c -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
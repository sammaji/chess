CC = gcc
CFLAGS = -Wall -Wextra -Wno-strict-prototypes -Wno-unused-parameter

SRCS = $(wildcard *.c) $(wildcard lib/*.c) $(wildcard tui/*c)
OBJS = $(SRCS:.c=.o)

TARGET = chess

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm -lc -lstdc++
	chmod +x $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

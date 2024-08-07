CC = gcc
CFLAGS = -Wall -Wextra -pedantic -Wno-c23-extensions -Wno-strict-prototypes -Wno-unused-parameter

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

TARGET = chess

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm
	chmod +x $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

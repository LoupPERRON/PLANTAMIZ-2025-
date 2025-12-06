CC = gcc
CFLAGS = -Wall -O2
SRCS = $(wildcard plantamitz/*.c)
OBJS = $(SRCS:.c=.o)

all: plantamitz.exe

plantamitz.exe: $(SRCS)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

clean:
	del plantamitz.exe 2>nul || true

CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -fmax-errors=3 -std=c++14 -g
TARGET = boog
SRCS = bintree.cc main.cc
OBJS = $(SRCS:.cc=.o)

all: $(TARGET)

clean:
	$(RM) *.o *~ $(TARGET)

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)



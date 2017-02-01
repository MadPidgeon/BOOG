CC = g++
CFLAGS = -Wall -Wextra -Wpedantic -Wconversion -fmax-errors=3 -std=c++14 -g
TARGET = boog
SRCS = bintree.cc main.cc counter.cc union_find.cc
OBJS = $(SRCS:.cc=.o)

all: $(TARGET)

gcst: gcst_test.cc bintree.o union_find.o
	$(CC) $(CFLAGS) -o gcst gcst_test.cc bintree.o union_find.o

proof: proof_test.cc bintree.o union_find.o
	$(CC) $(CFLAGS) -o proof proof_test.cc bintree.o union_find.o

clean:
	$(RM) *.o *~ $(TARGET)

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)



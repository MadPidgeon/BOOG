CXX = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -Wconversion -fmax-errors=3 -std=c++14 -g
TARGET = bin/boog
SRCS = bintree.cc main.cc counter.cc union_find.cc trie.cc counter.cc misc.cc
OBJS = $(SRCS:%.cc=obj/%.o)

all: $(TARGET)

bin/gcst: gcst_test.cc obj/bintree.o obj/union_find.o
	$(CXX) $(CXXFLAGS) -o bin/gcst gcst_test.cc obj/bintree.o obj/union_find.o

bin/proof: proof_test.cc obj/bintree.o obj/union_find.o
	$(CXX) $(CXXFLAGS) -o bin/proof proof_test.cc obj/bintree.o obj/union_find.o

bin/trie: trie_test.cc obj/trie.o
	$(CXX) $(CXXFLAGS) -o bin/trie trie_test.cc obj/trie.o

bin/json: basic_console.cc obj/bintree.o obj/union_find.o
	$(CXX) $(CXXFLAGS) -o bin/json basic_console.cc obj/bintree.o obj/union_find.o

bin/counter: counter_test.cc obj/bintree.o obj/union_find.o obj/counter.o obj/misc.o
	$(CXX) $(CXXFLAGS) -o bin/counter counter_test.cc obj/bintree.o obj/union_find.o obj/counter.o obj/misc.o

clean:
	$(RM) obj/*.o *~ bin/*

obj/%.o: %.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $< 

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)



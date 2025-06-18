CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

OBJS = MemoryManager.o
LIB = libMemoryManager.a
HEADERS = MemoryManager.h

all: $(LIB)

$(LIB): $(OBJS)
	ar rcs $(LIB) $(OBJS)

MemoryManager.o: MemoryManager.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c MemoryManager.cpp

clean:
	rm -f $(OBJS) $(LIB)

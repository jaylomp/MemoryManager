Memory Manager COP4600 Project 2
Overview
This project implements a custom dynamic memory manager in C++ that simulates heap memory allocation and deallocation using fixed-size word chunks. The memory manager uses a user-defined allocation strategy such as Best Fit or Worst Fit to manage memory holes and reduce fragmentation.

This was built as part of an operating systems course to reinforce concepts like memory layout, fragmentation, allocation strategies, and bitmap representation of memory.

Features
Custom Heap Management using fixed-size word allocations

Allocator Functions:

Best Fit

Worst Fit

Hole Tracking & Coalescing: Merges adjacent holes on free

Memory Dumping: Outputs memory map to file for inspection

Bitmap Generator: Returns a bitmap of allocated vs free words

Pluggable Allocator: Can switch allocation strategy at runtime

Technologies Used
C++17

POSIX File I/O (for dumpMemoryMap)

STL containers (vector, unordered_map)

Custom Makefile for compilation

Project Structure
bash
Copy
Edit
├── MemoryManager.h      # Interface and class definition
├── MemoryManager.cpp    # Implementation of allocation logic
├── Makefile             # Builds static library libMemoryManager.a

How to Build
bash
Copy
Edit
# Clone the repo
https://github.com/jaylomp/MemoryManager.git
cd memory-manager

# Build the static library
make
This will generate:

libMemoryManager.a: The static library

MemoryManager.o: Object file

To clean:

bash
Copy
Edit
make clean

Example Use
cpp
Copy
Edit
#include "MemoryManager.h"

int main() {
    // Use best-fit allocator
    MemoryManager manager(4, bestFit);
    manager.initialize(64); // 64 words

    void* ptr1 = manager.allocate(16);
    void* ptr2 = manager.allocate(8);
    manager.free(ptr1);

    manager.dumpMemoryMap("memory_map.txt");
}

Author
Jaylom Pairol

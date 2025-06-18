Memory Manager
COP4600 Project 2

Overview
This project implements a custom dynamic memory manager in C++ that simulates heap memory allocation and deallocation using fixed-size word chunks. It supports user-defined allocation strategies such as Best Fit and Worst Fit to manage memory holes and reduce fragmentation.

Built as part of an Operating Systems course, the project reinforces core concepts including memory layout, fragmentation, allocation strategies, and bitmap memory representation.

Features
Custom heap management with fixed-size word allocations

Allocation strategies:

Best Fit

Worst Fit

Hole tracking and coalescing (merging adjacent free blocks)

Memory dumping: outputs a memory map to a file for inspection

Bitmap generator: provides a bitmap representation of allocated vs free words

Pluggable allocator: allows switching allocation strategy at runtime

Technologies Used
C++17

POSIX File I/O (for dumping memory map)

STL containers (vector, unordered_map)

Custom Makefile for compilation

Project Structure
bash
Copy
Edit
├── MemoryManager.h        # Interface and class definitions  
├── MemoryManager.cpp      # Allocation and deallocation logic  
├── Makefile              # Builds static library libMemoryManager.a  
How to Build
bash
Copy
Edit
git clone https://github.com/jaylomp/MemoryManager.git
cd MemoryManager
make
This produces:

libMemoryManager.a — the static library

MemoryManager.o — the object file

To clean build files:

bash
Copy
Edit
make clean

Author: Jaylom Pairol

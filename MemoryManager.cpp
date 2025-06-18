#include "MemoryManager.h"
#include <cstring>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>
#include <climits>

// memory Hole with length and offset
Hole::Hole(size_t length, size_t offset) {
    this->length = length;
    this->offset = offset;
}

// comparison operator to sort for the holes
bool Hole::operator<(const Hole &other) const {
    return offset < other.offset;
}



// Constructor
MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator) {
    this->wordSize = wordSize;
    this->allocator = allocator;
    Block = nullptr;
    memLimit = 0;
}

// Destructor
MemoryManager::~MemoryManager() {
    shutdown();
}


// Initialize the memory manager
void MemoryManager::initialize(size_t sizeInWords) {
    if (Block != nullptr) {
        shutdown();
    }
    
    memLimit = sizeInWords * wordSize;
    Block = new uint8_t[memLimit];
    memset(Block, 0, memLimit);

    holes.clear();
    allocations.clear();


    holes.emplace_back(sizeInWords, 0);
}


// frees memory
void MemoryManager::shutdown() {
    if (Block) {
        delete[] Block;
        Block = nullptr;
    }

    memLimit = 0;
    holes.clear();
    allocations.clear();
}

// allocates memory and returns pointer to the space
void *MemoryManager::allocate(size_t sizeInBytes) {
    
    if (sizeInBytes == 0) {
        return nullptr;
    }

    size_t wordsNeeded = (sizeInBytes + wordSize - 1) / wordSize;

    void* Hole_list = getList();
    if (!Hole_list) {
        return nullptr;
    }

    int offset = allocator((int)wordsNeeded, Hole_list);
    delete[] (uint16_t*)Hole_list;

    if (offset < 0) return nullptr;

    for (auto it = holes.begin(); it != holes.end(); ++it) {
        if (it->offset == (size_t)offset) {
            if (it->length == wordsNeeded) {
                holes.erase(it);

            } else {
                it->offset += wordsNeeded;
                it->length -= wordsNeeded;
            }
            break;
        }
    }
    void* ptr = (void*)(Block + offset * wordSize);
    allocations[ptr] = wordsNeeded;
    return ptr;
}

// fress allocated memory but also merges adjecent holes
void MemoryManager::free(void* address) {
    if (!Block) {
        return;
    }

    if (!address) {
        return;
    }
 
    auto it = allocations.find(address);

    if (it == allocations.end()) {
        return;
    }

    size_t freedSize = it->second;
    allocations.erase(it);

    size_t freedBlockStart = ((uint8_t*)address - Block) / wordSize;
    holes.emplace_back(freedSize, freedBlockStart);
    std::sort(holes.begin(), holes.end());

    for (size_t i = 0; i < holes.size() - 1; ) {
        if (holes[i].offset + holes[i].length == holes[i + 1].offset) {
            
            holes[i].length += holes[i + 1].length;
            holes.erase(holes.begin() + i + 1);
        } else {
            ++i;
        }
    }
}

// returns a formatted list that represents the current holes in memory
void* MemoryManager::getList() {
    if (!Block) {
        return nullptr;
    }



    size_t numHoles = holes.size();
    size_t listSize = numHoles * 2 + 1;

    uint16_t* holeList = new uint16_t[listSize];
    holeList[0] = (uint16_t)numHoles;
    for (size_t i = 0; i < numHoles; ++i) {
        holeList[i * 2 + 1] = (uint16_t)holes[i].offset;
        holeList[i * 2 + 2] = (uint16_t)holes[i].length;
    }
    return (void*)holeList;
}

// dumps memory map to a file
int MemoryManager::dumpMemoryMap(char *filename) {
    if (!Block) {
        return -1;
    }

    if (!filename) {
        return -1;
    }

    int fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if (fd < 0) {
        perror("Error opening file");
        return -1;
    }

    std::ostringstream Mapstring;
    for (size_t i = 0; i < holes.size(); ++i) {
        const auto& hole = holes[i];
        Mapstring << "[" << hole.offset << ", " << hole.length << "]";
        if (i < holes.size() - 1) {
            Mapstring << " - ";
        }
    }

    std::string output = Mapstring.str();
    ssize_t bytesWritten = write(fd, output.c_str(), output.size());

    close(fd);
    return (bytesWritten == -1) ? -1 : 0;
}

// returns bitmap
void* MemoryManager::getBitmap() {
    if (!Block) {
        return nullptr;
    }

    size_t totalWords = memLimit / wordSize;
    size_t bitmapBytes = (totalWords + 7) / 8;
    uint8_t* result = new uint8_t[bitmapBytes + 2];

    result[0] = bitmapBytes & 0xFF;
    result[1] = (bitmapBytes >> 8) & 0xFF;
    memset(result + 2, 0, bitmapBytes);

    for (size_t i = 0; i < totalWords; ++i) {
        bool allocated = true;
        for (const auto &hole : holes) {
            if (i >= hole.offset && i < (hole.offset + hole.length)) {
                
                allocated = false;
                break;
            }
        }
        if (allocated) {
            int index = 2 + (i / 8);
            int bitpos = i % 8;
            result[index] |= (1 << bitpos);
        }
    }

    return result;
}


// update allocator
void MemoryManager::setAllocator(std::function<int(int, void*)> allocator) {
    this->allocator = allocator;
}

// returns word size
unsigned MemoryManager::getWordSize() {
    return wordSize;
}

// returns pointer to the start of the memory
void* MemoryManager::getMemoryStart() {
    return (void*)Block;
}

// returns the total size of managed memory
unsigned MemoryManager::getMemoryLimit() {
    return memLimit;
}

// finds the smallest hole big enough
int bestFit(int sizeInWords, void* list) {
    uint16_t* holes = (uint16_t*)list;

    int Offset_chosen = -1, bestSize = INT32_MAX;
    for (int i = 0; i < holes[0]; ++i) {
        int offset = holes[1 + 2 * i];
        int length = holes[2 + 2 * i];

        if (length >= sizeInWords && length < bestSize) {
            Offset_chosen = offset;
            bestSize = length;
        }
    }
    return Offset_chosen;
}

// finds the largest available hole
int worstFit(int sizeInWords, void* list) {
    uint16_t* holes = (uint16_t*)list;
    int worst = -1, worstSize = -1;

    for (int i = 0; i < holes[0]; ++i) {
        int offset = holes[1 + 2 * i];
        int length = holes[2 + 2 * i];

        if (length >= sizeInWords && length > worstSize) {
            worst = offset;
            worstSize = length;
        }
    }

    return worst;
}

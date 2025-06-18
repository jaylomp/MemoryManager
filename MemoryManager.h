#include <vector>
#include <functional>
#include <cstdint>
#include <unordered_map>


struct Hole {
    size_t length;
    size_t offset;

    Hole(size_t length, size_t offset);

    bool operator<(const Hole &other) const;
};

class MemoryManager {
public: 

    MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
    ~MemoryManager();

    void initialize(size_t sizeInWords);
    void shutdown();
    void *allocate(size_t sizeInBytes);
    void free(void *address);
    void setAllocator(std::function<int(int, void *)> allocator);
    int dumpMemoryMap(char *filename);
    
    void *getList();
    void *getBitmap();
    unsigned getWordSize();      
    void *getMemoryStart();      
    unsigned getMemoryLimit();   

private:
    unsigned wordSize;
    size_t memLimit;
    uint8_t* Block;
    std::function<int(int, void *)> allocator;
    std::vector<Hole> holes;
    std::unordered_map<void*, size_t> allocations;
};

int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);

#ifndef __engine__memory__
#define __engine__memory__

#define KB(n) n << 10
#define MB(n) n << 20
#define GB(n) n << 30

struct Allocator{
    void *(*allocate)(Allocator *allocator, u32 size);
    void (*deallocate)(Allocator *allocator, void *p);
};

struct NormalAllocator{
    Allocator allocator;
    NormalAllocator();
};

struct MemoryPool{
    Allocator allocator;
    
    uint64 size; // Reserved size
    uint64 occupied;
    uint64 highWaterMark;
    uint8 *memory;
    
    MemoryPool();
    MemoryPool(uint64 s);
    MemoryPool(uint64 s, uint8 *p);
};

extern Allocator defaultAllocator;
extern MemoryPool temporaryStorage;

#define start_temp_alloc() u64 __xdeg__startingTemporaryStorageMark = temporaryStorage.occupied
#define end_temp_alloc() temporaryStorage.occupied = __xdeg__startingTemporaryStorageMark
#define temp_alloc(size) temporaryStorage.allocator.allocate(&temporaryStorage.allocator, size)

void resetMemoryPool(MemoryPool &ma);
void freeMemory(MemoryPool ma);

void initEngineMemory();
void cleanupEngineMemory();
MemoryPool getMemoryPool(u64 size);

#endif /* defined(__engine__memory__) */

#include <assert.h>

#include "engine.hpp"
#include "memory.hpp"

MemoryPool totalHeapMemory;
u8 *totalHeapMemoryNextSection;

// Normal allocator
void * normal_allocate(Allocator *allocator, u32 size){
    if(size > 0){
        //printf("alloc\n"); // @Todo: Don't do so many allocations!!!!!!!!!!!!!!!!
        void *p = malloc(size);
        //printf("Allocating %p [size = %u]\n", p, size);
        return p;
    }else
        return nullptr;
}
void normal_deallocate(Allocator *allocator, void *p){
    //printf("Freeing %p\n", p);
    free(p);
}
NormalAllocator::NormalAllocator(){
    allocator.allocate = &normal_allocate;
    allocator.deallocate = &normal_deallocate;
}

Allocator defaultAllocator = {
    &normal_allocate,
    &normal_deallocate
};

MemoryPool temporaryStorage;

// Temp allocator
void * temp_allocate(Allocator *alloc, u32 s){
    MemoryPool *allocator = (MemoryPool *)alloc;
    u8 *t = allocator->memory+allocator->occupied;
    allocator->occupied += s;
    assert(allocator->occupied <= allocator->size);
    if(allocator->occupied > allocator->highWaterMark)
        allocator->highWaterMark = allocator->occupied;
    return t;
}
void temp_deallocate(Allocator *alloc, void *p){
    
}
void resetMemoryPool(MemoryPool &ma){
    ma.occupied = 0;
}
MemoryPool::MemoryPool(uint64 s){
    occupied = 0;
    highWaterMark = 0;
    size = s;
    memory = (uint8 *)malloc(s*sizeof(uint8));
    
    allocator.allocate = &temp_allocate;
    allocator.deallocate = &temp_deallocate;
}
MemoryPool::MemoryPool(uint64 s, uint8 *p){
    occupied = 0;
    highWaterMark = 0;
    size = s;
    memory = p;
    
    allocator.allocate = &temp_allocate;
    allocator.deallocate = &temp_deallocate;
}

MemoryPool::MemoryPool(){
    occupied = 0;
    highWaterMark = 0;
    size = 0;
    memory = nullptr;
    
    allocator.allocate = nullptr;
    allocator.deallocate = nullptr;
}

void freeMemory(MemoryPool ma){
    free(ma.memory);
}



void initEngineMemory(){
    totalHeapMemory = MemoryPool(20L << 20); // 20MB
    totalHeapMemoryNextSection = (u8 *) totalHeapMemory.memory;
    temporaryStorage = getMemoryPool(MB(10L));
}
MemoryPool getMemoryPool(u64 size){
    MemoryPool ma = MemoryPool(size, totalHeapMemoryNextSection);
    totalHeapMemoryNextSection += size;
    return ma;
}
void cleanupEngineMemory(){
    freeMemory(totalHeapMemory);
}

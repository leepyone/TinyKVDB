//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_MEMORY_POOL_H
#define TINYKVDB_MEMORY_POOL_H
#include "common.h"

class MemoryPool{
public:
    explicit MemoryPool(int ttl_msec);
    ~MemoryPool();

    char* alloc(size_t size);
    void dealloc(char *p, size_t size);
    char* dup(const char* p ,size_t size);

    MemoryPool(const MemoryPool&) = delete;
    MemoryPool operator=(const MemoryPool&) =delete;

private:
    struct Block{
        char *addr;
        std::chrono::time_point<std::chrono::steady_clock> expire_at;
    };

    enum{
        ALIGN =8,
        MAX_BLOCK_SIZE = 256,
    };

    int _ttl_msec;

    std::deque<Block> _block_queue[MAX_BLOCK_SIZE/ALIGN];
    std::queue<Block> _block_persist;
    std::unordered_set<char*> _blocks; //记录分配空间的位置

    char* alloc_small(size_t size);
    char* alloc_large(size_t size);

    void dealloc_small(char* p, size_t size);
    void dealloc_large(char* p, size_t size);
    void refill(std::deque<Block> *que, size_t size);
    void free_expired_block();

};


#endif//TINYKVDB_MEMORY_POOL_H

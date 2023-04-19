//
// Created by Sure on 2023/4/14.
//
#include "../include/memory_pool.h"

static inline size_t round_up(size_t n, size_t align){
    return (n+align-1)& ~(align-1); // todo ?
}

MemoryPool::MemoryPool(int ttl_msec): _ttl_msec(ttl_msec) {}

MemoryPool::~MemoryPool() {
    for(const auto& p:_blocks){
        delete[] p;
    }
}

char* MemoryPool::alloc(size_t size) {
    free_expired_block();

    size = round_up(size, ALIGN);

    if(size>MAX_BLOCK_SIZE)
        return alloc_large(size);
    return alloc_small(size);
}

char* MemoryPool::alloc_small(size_t size) {
    size_t index = size/ALIGN -1;

    if(_block_queue[index].empty() ||
        _block_queue[index].front().expire_at>std::chrono::steady_clock::now()){//这个位置为空或者这个block过期了
        refill(&_block_queue[index],size);
    }
    char *addr = _block_queue[index].front().addr;
    _block_queue[index].pop_front();//弹出 取出这个空间
    return addr;
}

char* MemoryPool::alloc_large(size_t size) {// 分配过大的空间直接进行分配，不用分block
    char *p = new char[size];
    _blocks.insert(p);
    return p;
}

void MemoryPool::dealloc(char *p, size_t size) {
    free_expired_block();

    size = round_up(size, ALIGN);
    if(size>MAX_BLOCK_SIZE){
        return dealloc_large(p,size);
    }
    return dealloc_small(p,size);
}
void MemoryPool::dealloc_small(char *p, size_t size) {
    size_t index = size/ALIGN -1;
    _block_queue[index].emplace_back(Block{p,std::chrono::steady_clock::now()+std::chrono::milliseconds(_ttl_msec)});
}

void MemoryPool::dealloc_large(char *p, size_t size) {
    _block_persist.emplace(Block{p,std::chrono::steady_clock::now()+std::chrono::milliseconds(_ttl_msec)});
    (void)size;
}

char* MemoryPool::dup(const char *p, size_t size) {
    char* new_p = alloc(size);
    memcpy(new_p,p,size);
    return new_p;
}


void MemoryPool::refill(std::deque<Block> *que, size_t size) {
    static constexpr int NBLOCK = 20;

    char *p = new char[size* NBLOCK];
    _blocks.insert(p);
    auto now = std::chrono::steady_clock::now();

    for(int i=0;i<NBLOCK;i++){
        que->emplace_front(Block{p,now});//emplace_front 在队头添加元素 每次添加一个size大小的block
        p+=size;
    }
}


void MemoryPool::free_expired_block() {
    auto now = std::chrono::steady_clock::now();

    while(!_block_persist.empty()){
        const Block& front = _block_persist.front();
        if(front.expire_at>now){
            break ;
        }

        delete[] front.addr;
        _blocks.erase(front.addr);
        _block_persist.pop();
    }
}
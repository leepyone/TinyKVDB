//
// Created by Sure on 2023/4/14.
//

#include "../include/skiplist.h"

namespace table {
}// namespace table
SkipList::Iterator::Iterator() : _node(nullptr) {}
SkipList::Iterator::Iterator(Node *node) : _node(node){}
SkipList::Iterator::~Iterator() {}

void SkipList::Iterator::next() {_node=_node->next[0];}
bool SkipList::Iterator::good() {return _node!= nullptr;}
const ByteArray& SkipList::Iterator::key(){return _node->key;}
const ByteArray& SkipList::Iterator::value() {return _node->value;}

//初始化构造
SkipList::SkipList(Comparator *cmp, MemoryPool *pool): _height(1),_head(nullptr), _rand(RANDOM_SEED),_cmp(cmp),_pool(pool) {
    _head = new_node("head","head",MAX_HEIGHT);
}

SkipList::Iterator SkipList::begin() {
    return Iterator(_head->next[0]);
}

SkipList::Iterator SkipList::insert(const ByteArray &key, const ByteArray &value) {
    Node *prev[MAX_HEIGHT] = { nullptr};
    Node *node = first_greater_or_equal(key,prev);

    if(node && _cmp->compare(node->key,key)==0){
        return Iterator(nullptr);
    }

    int new_height = random_height();

    Node *insertNode = new_node(key,value,new_height);

    if(new_height>_height){
        for(int i=_height;i<new_height;i++){
            prev[i]=_head;
        }
        _height= new_height;
    }

    publish_node(insertNode,prev);

    return Iterator(insertNode);
}

SkipList::Iterator SkipList::update(const ByteArray &key, const ByteArray &new_value) {
    Node *prev[MAX_HEIGHT] ={ nullptr};
    Node *node = first_greater_or_equal(key,prev);

    if(node && node!=_head&&_cmp->compare(node->key,key)==0){

        Node *insert_node = new_node(node->key,new_value,node->height);
        Node *temp[MAX_HEIGHT];
        std::fill_n(temp,node->height,node);
        publish_node(insert_node,temp);

        remove_node(node ,prev);
        delete_node(node);

        return Iterator(insert_node);

    }

    return Iterator(nullptr);

}

SkipList::Iterator SkipList::lookup(const ByteArray &key) {//寻找key，找到返回迭代器
    Node *node = first_greater_or_equal(key, nullptr);
    if(node && _cmp->compare(node->key,key)==0){
        return Iterator(node);
    }
    return Iterator(nullptr);

}

bool SkipList::remove(const ByteArray &key) {
    Node *prev[MAX_HEIGHT] = { nullptr};
    Node *node = first_greater_or_equal(key,prev);

    if(node &&_cmp->compare(node->key,key)==0){
        remove_node(node,prev);
        delete_node(node);
        return true;
    }

    return false;
}

/**
 * 随机返回这个节点的level
 * @return
 */
int SkipList::random_height() {
    int height =1;
    while(height<static_cast<int>(MAX_HEIGHT) && (_rand.rand()&(GROWTH_PROBABILITY-1))==0){
        height++;
    }
    return  height;
}

SkipList::Node* SkipList::new_node(const ByteArray &key, const ByteArray &value, int height) {
        ByteArray new_key(_pool->dup(key.data(),key.size()),key.size());
        ByteArray new_value(_pool->dup(value.data(),value.size()),value.size());

        // 先分配空间
        void *p = _pool->alloc(sizeof(Node)+ sizeof(Node*)*(height-1));
        // 使用这个空间来创建节点指针
        Node *node = new(p) Node(height, new_key, new_value);

        return node;
}
void SkipList::delete_node(SkipList::Node *node) {
    _pool->dealloc(node->key.data(),node->key.size());
    _pool->dealloc(node->value.data(),node->value.size());
    _pool->dealloc(reinterpret_cast<char*>(node),sizeof(Node)+sizeof(Node*)*(node->height-1));

}

void SkipList::publish_node(SkipList::Node *node, SkipList::Node **prev) {
    for(int i=0;i<node->height;i++){
        // 有点类似上锁和释放
        std::atomic_thread_fence(std::memory_order_acquire);

        node->next[i] = prev[i]->next[i];
        prev[i]->next[i] =node ;
        std::atomic_thread_fence(std::memory_order_release);
    }
}

void SkipList::remove_node(SkipList::Node *node, SkipList::Node **prev) {
    for(int i=node->height-1;i>=0;i--){
        prev[i]->next[i] = node->next[i];
    }
}

SkipList::Node* SkipList::first_greater_or_equal(const ByteArray &key, SkipList::Node **prev) {
    int height = _height-1;
    Node *prev_node = _head;
    while(height>=0){//一直向下走
        Node *next_node = prev_node->next[height];
        while(next_node &&_cmp->compare(next_node->key,key)<0){
            prev_node = next_node;
            next_node = next_node->next[height];
        }

        if(prev){//记录每一层索引对应的节点
            prev[height] = prev_node;
        }
        if(height==0){
            break ;
        }
        --height;

    }
    return prev_node->next[0]; //prev_node 就是我们要寻找的目标节点的前一个节点
}
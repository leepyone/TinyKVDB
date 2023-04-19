//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_SKIPLIST_H
#define TINYKVDB_SKIPLIST_H
#include "byte_array.h"
#include "random.h"
#include "options.h"
#include "memory_pool.h"


class SkipList {
private:
    struct Node;
public:
    class Iterator{
    public:
        Iterator();
        Iterator(Node *node);
        ~Iterator();

        bool good();

        void next();

        const ByteArray& key();
        const ByteArray& value();
    private:
        Node* _node;
    };

    SkipList(Comparator *cmp,MemoryPool *pool);
    ~SkipList() = default;

    Iterator begin();
    Iterator insert(const ByteArray& key, const ByteArray& value);
    Iterator update(const ByteArray& key, const ByteArray& new_value);
    Iterator lookup(const ByteArray& key);

    bool remove(const ByteArray& key);

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) =delete;


private:
    struct Node{
        Node(int h, const ByteArray& k, const ByteArray& v):height(h),key(k),value(v){ std::fill_n(next, h, nullptr);};

        int height;
        ByteArray key;
        ByteArray value;
        Node* next[1];//这里分配的空间是1 todo
    };

    enum{
        MAX_HEIGHT = 16,
        RANDOM_SEED = 0xBADC0FFE,
        GROWTH_PROBABILITY = 4,
    };

    int _height;
    Node* _head;
    Random _rand;
    Comparator *_cmp;
    MemoryPool *_pool;

    int random_height();
    Node* first_greater_or_equal(const ByteArray& key, Node **prev);

    Node* new_node(const ByteArray& key, const ByteArray& value, int height);

    void delete_node(Node* node);
    void publish_node(Node* node, Node** prev);
    void remove_node(Node* node, Node** prev);


};


#endif//TINYKVDB_SKIPLIST_H

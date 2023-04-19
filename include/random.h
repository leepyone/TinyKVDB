//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_RANDOM_H
#define TINYKVDB_RANDOM_H
#include "../include/common.h"


class Random{
public:
    explicit Random(uint32_t seed): _seed(seed){};
    ~Random() = default;

    uint32_t rand(){
        static const uint64_t M = 2147483647;
        static const uint64_t A = 2147483629;
        static const uint64_t C = 2147483587;

        _seed = (static_cast<uint64_t>(_seed) *A+C) % M; // todo 有什么用 一个32 一个64 ?

        return _seed;
    }

private:
    uint32_t _seed;
};


#endif//TINYKVDB_RANDOM_H

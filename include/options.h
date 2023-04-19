//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_OPTIONS_H
#define TINYKVDB_OPTIONS_H
#include "byte_array.h"

class Comparator{
public:
    virtual int compare(const ByteArray& lhs, const ByteArray& rhs) const=0;
};

struct Options{
    Comparator* comparator;
    bool create_if_missing;
    bool error_if_exists;
    bool dump_when_close;
    int read_ttl_mesc;
    off_t  max_file_size;

    Options();
};

#endif//TINYKVDB_OPTIONS_H

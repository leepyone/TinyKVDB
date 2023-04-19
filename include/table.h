//
// Created by Sure on 2023/4/19.
//

#ifndef TINYKVDB_TABLE_H
#define TINYKVDB_TABLE_H

#include "status.h"
#include "options.h"
#include "byte_array.h"

class Table{
public:
    Table(const Options& options ,const std::string& filename);

    ~Table();

    Status open();

    Status close();

    Status dump();

    Status get(const ByteArray& key, std::string* value);

    Status put(const ByteArray& key, const ByteArray& value);

    Status del(const ByteArray& key);

    Table(const Table&) = delete;
    Table& operator=(const Table&) = delete;

private:
    class TableImpl;
    TableImpl *_impl;


};


#endif//TINYKVDB_TABLE_H

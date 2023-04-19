//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_BYTE_ARRAY_H
#define TINYKVDB_BYTE_ARRAY_H

#include <string>

class ByteArray{
public:
    ByteArray();
    ~ByteArray();
    ByteArray(const char* data, size_t size);
    ByteArray(const char* data);
    ByteArray(const std::string& s);

    bool empty() const;
    size_t size() const;
    char* data();
    const char* data() const;
    void assign(const char* data, size_t size);

private:
    size_t _size;
    const char* _data;

};


#endif//TINYKVDB_BYTE_ARRAY_H

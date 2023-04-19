//
// Created by Sure on 2023/4/14.
//

#include "../include/byte_array.h"
#include "../include/common.h"

bool operator ==(const ByteArray& lhs, const ByteArray& rhs){
    if(lhs.data()==rhs.data())
        return true;

    if(lhs.size()!=rhs.size())
        return false;

    return memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
}

ByteArray::ByteArray() :_size(0),_data(""){}
ByteArray::ByteArray(const char *data, size_t size): _size(size),_data(data){}
ByteArray::ByteArray(const char *data) :_size(strlen(data)), _data(data){}
ByteArray::ByteArray(const std::string& s) :_size(s.size()),_data(s.data()){}
ByteArray::~ByteArray() {}

bool ByteArray::empty() const { return _size==0;}

size_t ByteArray::size() const {return _size;}

char* ByteArray::data()  { return const_cast<char*>(_data);} // todo const_cast

const char* ByteArray::data() const { return _data;}

void ByteArray::assign(const char *data, size_t size) { _data=data; _size=size;}
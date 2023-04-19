//
// Created by Sure on 2023/4/14.
//

#include "../include/options.h"
#include "../include/common.h"

class ByteWiseComparator: public Comparator{
public:
    int compare(const ByteArray &lhs, const ByteArray &rhs) const override {
        size_t size =std::min(lhs.size(),rhs.size());

        int cmp = memcmp(lhs.data(),rhs.data(),size);
        if(cmp!=0)
            return cmp;

        return static_cast<int>(lhs.size())-static_cast<int>(rhs.size());
    }
};

static Comparator* default_comparator(){
    static ByteWiseComparator cmp;
    return &cmp;
}

Options::Options():
                     comparator(default_comparator()),
                     create_if_missing(false),
                     error_if_exists(false),
                     dump_when_close(true),
                     read_ttl_mesc(2000),
                     max_file_size(1024*1024*1024)
{

}
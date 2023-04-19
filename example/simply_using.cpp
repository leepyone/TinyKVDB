//
// Created by Sure on 2023/4/19.
//
#include <iostream>

#include "../include/table.h"

static inline void assert_fatal(const Status& s) {
    if (!s.good()) {
        std::cerr << s.string() << std::endl;
        exit(-1);
    }
}

int main() {
    Options options;
    options.create_if_missing = true;
    Table table(options, "test_table");

    Status s = table.open();
    assert_fatal(s);

    table.close();
    return 0;
}
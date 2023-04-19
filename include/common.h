//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_COMMON_H
#define TINYKVDB_COMMON_H

#ifdef GTEST
#define TABLE_PUBLIC public
#define TABLE_PROTECT public
#define TABLE_PRIVATE public
#else
#define TABLE_PUBLIC public
#define TABLE_PROTECT protect
#define TABLE_PRIVATE private
#endif

#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/mman.h>
#ifdef __APPLE__
#include <sys/syslimits.h>
#else
#include <linux/limits.h>
#endif

#include <queue>
#include <deque>
#include <atomic>
#include <chrono>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#endif//TINYKVDB_COMMON_H

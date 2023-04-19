## 基于跳表实现的轻量级键值型内存存储引擎

本项目用C++实现基于跳表实现的轻量级键值型内存存储引擎。

#### 功能：
* Key 和 value 是任意字节的数组，自己实现的 byte_array 类，使用浅拷贝来增加节点复制速度；
* 数据是按 Key 字典序排序存储在跳表中的；
* 使用内存池来进行内存的分配与释放；
* 支持 CRUD 基本操作如：put(key , value) , get(key) , del(key) ;
* 支持数据持久化到磁盘上，但是不支持 `crash-safe 崩溃恢复`；



### 示例：
Opening or Closing a Table
```C++
table::Options options;
options.create_if_missing = true;
table::Table table(options, "test_table");

table::Status s = table.open();
if(!s.good()) {
    std::cerr << s.string() << std::endl;
}

table.close();
```

Read、Write、Detele
```C++
std::string key = "key";
std::string value = "value";

s = table.put(key, value);
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}

s = table.get(key, &value);
if (!s.good()) {
    std::cerr << s.string() << std::endl;
} else {
    std::cout << value << std::endl;
}

s = table.del(key) ; 
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}
```
Persisting data
```C++
std::string key = "key";
std::string value = "value";

s = table.put(key, value);
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}

s = table.dump();
if (!s.good()) {
    std::cerr << s.string() << std::endl;
}
```


### TODO 优化

- [ ] byte_array 结构体优化，设计不同的结构头，如内部表示字符长度可以是：uint16_t 、uint32_t、uint64_t 等，现在只是 uint8_t 一个，这样的话字符串的长度必须小于 2^8 。
- [ ] 可以加布隆过滤器，加快判断 key 是否在内存里


### 参考代码库
1. [使用C++开发，基于跳表实现的轻量级键值数据库](https://github.com/youngyangyang04/Skiplist-CPP)
2. [Table is an in memory key-value store library](https://github.com/wonter/table)
3. [leveldb_comment](https://github.com/TitenWang/leveldb-comment)

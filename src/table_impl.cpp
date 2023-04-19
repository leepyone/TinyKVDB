//
// Created by Sure on 2023/4/19.
//

#include "../include/table.h"

#include "../include/skiplist.h"
#include "../include/memory_pool.h"

class Table::TableImpl{
public:
    TableImpl(const Options& options, const std::string& filename);
    ~TableImpl();

    Status open();
    Status close();

    Status dump();

    Status get(const ByteArray& key, std::string* value);
    Status put(const ByteArray& key, const ByteArray& value);
    Status del(const ByteArray& key);

    TableImpl(const TableImpl&) = delete;
    TableImpl& operator=(const TableImpl&) = delete;
private:
    bool _is_closed;
    Options _options;
    MemoryPool _pool;
    SkipList _skipList;
    std::string _name;
};

Table::TableImpl::TableImpl(const Options &options, const std::string &filename) :_is_closed(true),_options(options),_pool(options.read_ttl_mesc),
                                                                                   _skipList(options.comparator,&_pool),_name(filename){
}
Table::TableImpl::~TableImpl() {
    close();
}
Status Table::TableImpl::open() {
    if(!_is_closed){
        return Status::invalid_operation("表格已经开启了");
    }
    struct stat info;

    bool table_exit = stat(_name.c_str(),&info)==0;
    if(table_exit && _options.error_if_exists){
        return Status::io_error(_name+" 不存在");
    }
    if(mkdir(_name.c_str(),0755)){
        return Status::io_error(strerror(errno));
    }
    auto closedir_func = [](DIR* d){
        if(d){
            closedir(d);
        }
    };
    std::shared_ptr<DIR> directory(opendir(_name.c_str()), closedir_func);
    if(directory== nullptr){
        return Status::io_error(" 不能打开 "+_name+" 文件夹");
    }

    errno =0;
    struct dirent *entry;
    char path[PATH_MAX];
    for(entry = readdir(directory.get()); entry!= nullptr;entry = readdir(directory.get())){
        snprintf(path, PATH_MAX, "%s/%s",_name.c_str(),entry->d_name);

        if(strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0){
            continue ;
        }

        if(stat(path,&info)!=0 || !(info.st_mode & S_IFREG)){
            continue ;
        }

        auto close_func = [](int* fd){
            if(fd){
                ::close(*fd);
                delete fd;
            }
        };

        std::shared_ptr<int> fd(new int(::open(path,O_RDONLY)),close_func);
        if(*fd == -1){
            return Status::io_error("open "+ std::string(path) + " error, "+ strerror(errno));
        }

        if (info.st_size > _options.max_file_size){
            return Status::io_error("文件 "+ std::string(path)+ " 太大了， "+" 最大长度为 "+ std::to_string(_options.max_file_size));
        }

        if( info.st_size>0){
            auto munmap_func =[&info](char *data){
                if(data !=MAP_FAILED){
                    munmap(data, info.st_size);
                }
            };
            std::shared_ptr<char> data(
                    reinterpret_cast<char*>(
                            mmap(nullptr, info.st_size, PROT_READ, MAP_PRIVATE, *fd, 0)),
                    munmap_func);
            if (data.get() == MAP_FAILED) {
                return Status::io_error("mmap " + std::string(path) + " error, " + strerror(errno));
            }
            off_t offset = 0;
            while (true) {
                ByteArray key;
                ByteArray value;
                if (info.st_size - offset >= static_cast<off_t>(sizeof(size_t))) {
                    size_t size = *reinterpret_cast<size_t *>(data.get() + offset);
                    key.assign(data.get() + offset + sizeof(size_t), size);
                    offset += sizeof(size_t) + size;
                }
                if (info.st_size - offset >= static_cast<off_t>(sizeof(size_t))) {
                    size_t size = *reinterpret_cast<size_t *>(data.get() + offset);
                    value.assign(data.get() + offset + sizeof(size_t), size);
                    offset += sizeof(size_t) + size;
                }
                if (key.empty() || value.empty()) {
                    break;
                }
                auto it = _skiplist.insert(key, value);
                if (!it.good()) {
                    return Status::invalid_operation(
                            "duplicate key " + std::string(key.data(), key.size()));
                }
            }
        }
        if (errno != 0) {
            return Status::io_error("readdir " + _name + " error, " + strerror(errno));
        }

        _is_closed = false;
        return Status::ok();
    }


}

Status Table::TableImpl::close() {
    if (_is_closed) {
        return Status::invalid_operation("Table is closed");
    }

    if (_options.dump_when_close) {
        Status s = dump();
        if (!s.good()) {
            return s;
        }
    }

    _is_closed = true;
    return Status::ok();
}

Status Table::TableImpl::dump() {
    if (_is_closed) {
        return Status::invalid_operation("Table is closed");
    }

    auto close_func = [](int* fd) {
        if (fd) {
            ::close(*fd);
            delete fd;
        }
    };

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s", _name.c_str());

    off_t bytes = 0;
    int split_num = 0;
    std::shared_ptr<int> fd;
    for (auto it = _skiplist.begin(); it.good(); it.next()) {
        size_t entry_size = it.key().size() + it.value().size() + sizeof(size_t) * 2;
        if (bytes + static_cast<off_t>(entry_size) > _options.max_file_size) {
            bytes = 0;
        }

        if (bytes == 0) {
            snprintf(path + _name.size(), PATH_MAX - _name.size(), "/%08X", split_num);
            fd.reset(new int(::open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666)), close_func);
            if (*fd == -1) {
                return Status::io_error("open " + std::string(path) + " error, " + strerror(errno));
            }
            ++split_num;
        }

        // +--------------------Entry----------------------+
        // | length of key | key | length of value | value |
        // +-----------------------------------------------+
        std::shared_ptr<char> buffer(new char[entry_size]);
        size_t offset = 0;
        size_t size = it.key().size();
        memcpy(buffer.get() + offset, &size, sizeof(size));
        offset += sizeof(size);
        memcpy(buffer.get() + offset, it.key().data(), size);
        offset += size;
        size = it.value().size();
        memcpy(buffer.get() + offset, &size, sizeof(size));
        offset += sizeof(size);
        memcpy(buffer.get() + offset, it.value().data(), size);

        if (write(*fd, buffer.get(), entry_size) == -1) {
            return Status::io_error("write " + std::string(path) + " error, " + strerror(errno));
        }

        bytes += entry_size;
    }

    // just remove the superfluous files
    struct stat _;
    while (true) {
        snprintf(path + _name.size(), PATH_MAX - _name.size(), "/%08X", split_num);
        if (stat(path, &_) != 0) {
            break;
        }

        if (remove(path) == -1) {
            return Status::io_error(strerror(errno));
        }
        ++split_num;
    }

    return Status::ok();
}

Status Table::TableImpl::get(const ByteArray& key, std::string* value) {
    if (_is_closed) {
        return Status::invalid_operation("Table is closed");
    }

    auto it = _skipList.lookup(key);
    if (!it.good()) {
        return Status::not_found();
    }

    if (value != nullptr) {
        value->assign(it.value().data(), it.value().size());
    }
    return Status::ok();
}

Status Table::TableImpl::put(const ByteArray& key, const ByteArray& value) {
    if (_is_closed) {
        return Status::invalid_operation("Table is closed");
    }

    size_t entry_size = key.size() + value.size() + sizeof(size_t) * 2;
    if (static_cast<off_t>(entry_size) > _options.max_file_size) {
        return Status::invalid_operation("size of entry is too large");
    }

    auto it = _skipList.insert(key, value);
    if (!it.good()) {
        _skipList.update(key, value);
    }

    return Status::ok();
}

Status Table::TableImpl::del(const ByteArray& key) {
    if (_is_closed) {
        return Status::invalid_operation("Table is closed");
    }

    if (_skipList.remove(key)) {
        return Status::ok();
    } else {
        return Status::not_found();
    }
}

Table::Table(const Options& options, const std::string& filename)
    : _impl(new TableImpl(options, filename)) { }
Table::~Table() { delete _impl; }
Status Table::open() { return _impl->open(); }
Status Table::close() { return _impl->close(); }
Status Table::dump() { return _impl->dump(); }
Status Table::get(const ByteArray& key, std::string* value) { return _impl->get(key, value); }
Status Table::put(const ByteArray& key, const ByteArray& value) { return _impl->put(key, value); }
Status Table::del(const ByteArray& key) { return _impl->del(key); }

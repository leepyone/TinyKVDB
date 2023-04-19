//
// Created by Sure on 2023/4/14.
//

#ifndef TINYKVDB_STATUS_H
#define TINYKVDB_STATUS_H

#include <string>

class Status{
public:
    enum Code{
        OK =0,
        IO_ERROR=1,
        NOT_FOUND=2,
        INVALID_OPERATION=3,
    };

    Status();
    explicit Status(Code code);
    Status(Code code, const std::string& msg);
    ~Status() = default;

    bool good() const;
    Code code() const;
    std::string string() const;

    static Status ok(const std::string& msg);
    static Status io_error(const std::string& msg);
    static Status not_found(const std::string& msg);
    static Status invalid_operation(const std::string&msg);
    static Status ok();
    static Status io_error();
    static Status not_found();
    static Status invalid_operation();

private:
    Code _code;
    std::string _msg;

};

#endif//TINYKVDB_STATUS_H

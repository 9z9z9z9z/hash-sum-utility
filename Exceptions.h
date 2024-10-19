#pragma once
#include <exception>
#include <string>

namespace Exceptions {

class BasicException : public std::exception {
public: 
    explicit BasicException(const std::string &mes) {
        _message = mes;
    }

    const char *what() const noexcept override {
        return _message.c_str();
    }
private:
    std::string _message;
};

class FILE_OPEN_ERR : public BasicException {
public:
    explicit FILE_OPEN_ERR(const std::string &mes) : BasicException(mes) {}
};

class FILE_SIZE_ERR : public BasicException {
public:
    explicit FILE_SIZE_ERR(const std::string &mes) : BasicException(mes) {}
};

class JSON_VAL_EXTRACT_ERR : public BasicException {
public:
    explicit JSON_VAL_EXTRACT_ERR(const std::string &mes) : BasicException(mes) {}
};

class JSON_FORMAT_ERR : public BasicException {
public:
    explicit JSON_FORMAT_ERR(const std::string &mes) : BasicException(mes) {}
};
}
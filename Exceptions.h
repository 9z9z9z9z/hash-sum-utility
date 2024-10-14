#ifndef _EXCEPTIONS_
#define _EXCEPTIONS_

#include <exception>
#include <string>

namespace Exceptions {

class FILE_OPEN_ERR : public std::exception {
    public:
        FILE_OPEN_ERR(const std::string &messange) {
            _messange = messange;
        }
        const char* what() const noexcept override {
            return _messange.c_str();
        }
    private:
        std::string _messange;
};

class FILE_SIZE_ERR : public std::exception {
    public:
        FILE_SIZE_ERR(const std::string &messange) {
            _messange = messange;
        }
        const char* what() const noexcept override {
            return _messange.c_str();
        }
    private:
        std::string _messange;
};
class JSON_VAL_EXTRUCT_ERR : public std::exception {
    public:
        JSON_VAL_EXTRUCT_ERR(const std::string &message) {
            _message = message;
        }
        const char* what () const noexcept override {
            return _message.c_str();
        }
    private:
        std::string _message;
};

class JSON_IN_LINE_ERR : public std::exception {
    public:
        JSON_IN_LINE_ERR(const std::string &message) {
            _message = message;
        }
        const char* what () const noexcept override {
            return _message.c_str();
        }
    private:
        std::string _message;
};
}
#endif
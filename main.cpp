#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

const uint32_t startingPoly = 0xEDB88320;

class Buffer {
public:
    uint32_t _buffer;

    Buffer() {
        _buffer = 0;
    }
    Buffer(uint32_t buff) {
        _buffer = buff;
    }
    Buffer(const Buffer &copy) {
        _buffer = copy._buffer;
    }

    bool isOne() {
        return (_buffer & _mask) == _mask;
    }

    ~Buffer() {};

private:
    int32_t _mask = 0x8000;
};

// #ifdef CRC32
uint32_t crc32(const void *data, size_t size) {
    uint32_t ret = -1;
    const unsigned char *buff = reinterpret_cast<const unsigned char*>(data);
    while (size--) {
        ret = ret ^ *buff++;
        for (int bit = 0; bit < 8; ++bit) {
            if (ret & 1) {
                ret = (ret >> 1) ^ startingPoly;
            } else {
                ret = (ret >> 1);
            }
        }        
    }

    return ~ret;
}
// #endif

int main(int argc, char **argv) {
    std::string str = "";
    for (size_t i = 1; i < argc; ++i) {
        str += argv[i];
        if (argc > 2) {
            str += " ";
        }
    }
    std::cout << str << '\n';
    std::cout << std::hex << crc32(str.c_str(), str.length()) << std::endl;
    return 0;
}
#include "stdafx.h"

#include "test/test.hpp"

#include "crc32/crc.hpp"


int main(int argc, char **argv) {
    if (std::string(argv[1]) == "-test") {
        return test();
    }
    
    try {
        if (CRC::hashSumChecking(argv[1])) {
            std::cout << KGRN << "\nAll files are correct\n" << RST << std::endl;
        } else {
            std::cout << KRED << "\nSome files were changed\n" << RST << std::endl;
        }
    } catch (Exceptions::FILE_OPEN_ERR foEx) {
        std::cerr << KRED << foEx.what() << '\n' << RST;
    }
        
    return 0;
}
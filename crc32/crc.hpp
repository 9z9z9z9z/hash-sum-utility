#pragma once
#include "../stdafx.h"

#include "../file_funcs/file_funcs.hpp"

namespace CRC {
    uint32_t crc32(const void *data, uint32_t lastVal, size_t size) {
        const uint32_t startingPoly = 0xEDB88320;
        uint32_t hashSum = ~lastVal;
        const unsigned char *buff = reinterpret_cast<const unsigned char*>(data);

        while (size--) {
            hashSum = hashSum ^ uint32_t(*buff);
            ++buff;
            for (uint8_t bit = 0; bit < 8; ++bit) {
                if ((hashSum & uint32_t(1)) != 0) {
                    hashSum = (hashSum >> 1) ^ startingPoly;
                } else {
                    hashSum = (hashSum >> 1);
                }
            }
        }

        return ~hashSum;
    }

    bool checkCurrentFile(const std::string &filePath, const uint32_t hashSum) {
        size_t currentBit = 0;
        std::ifstream file(filePath, std::ifstream::binary);
        if (!file.is_open()) {
            throw Exceptions::FILE_OPEN_ERR("Cannot open " + filePath + " file\n");
        }
        uint32_t hash = 0x00000000;
        size_t buffSize = 4096; // 4 KB buffer    
        size_t readedBytes;

        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(buffSize);
        while (!file.eof()) {
            file.read(buffer.get(), buffSize);
            readedBytes = file.gcount();
            if (readedBytes > 0) {
                hash = CRC::crc32(buffer.get(), hash, readedBytes);
            }
        }

        if (hash != hashSum) {
            std::cerr << KRED   << "\nFile " << filePath << " was changed\n";
            std::cerr           << "Calculated hash is " << std::hex << hash
                                << " expected " << std::hex << hashSum << '\n' << RST;
            return false;
        } else {
            std::cout << KGRN   << "\nFile " << filePath << " is correct\n";
            std::cout           << "Calculated hash is " << std::hex << hash
                                << " expected " << std::hex << hashSum << '\n' << RST;
            return true;
        }
    }
    
    bool hashSumChecking(const std::string &jsonFilePath) {
        bool ret = true;
        try {
            std::vector<FilesFuncs::JsonFile> files = FilesFuncs::parseJsonFile(jsonFilePath);
            if (files.empty()) {
                return false;
            }
            for (std::vector<FilesFuncs::JsonFile>::iterator it = files.begin(); it != files.end(); ++it) {
                try {
                    if (!checkCurrentFile(it->filePath, it->hashSum)) {
                        ret = false;
                    }
                } catch (...) {
                    continue;
                }
            }
            return ret;
        } catch (...) {
            return false;
        }  
    }

}

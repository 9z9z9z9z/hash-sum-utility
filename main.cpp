#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

#include "Exceptions.h"
#include "colors.h"
#include "json.hpp"

const static std::string PATH = "path";
const static std::string HASH = "hash";

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
}

namespace FilesFuncs {
    struct JsonFile {
        std::string filePath;
        uint32_t hashSum;
        JsonFile() {}
        JsonFile(const std::string &rFilePath, uint32_t rHashSum) {
            filePath = rFilePath;
            hashSum = rHashSum;
        }
    };

    size_t fileSize(const std::string &filePath) { 
        try {
            std::ifstream file(filePath, std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                size_t ret = file.tellg();
                file.close();
                return ret;
            } else {
                 return -1;
            }            
        } catch (...) {
            return -1;
        }
    }

    bool checkCurrentFile(const std::string &filePath, const uint32_t hashSum) {
        size_t currentBit = 0;
        std::ifstream file(filePath, std::ifstream::binary);
        if (!file.is_open()) {
            throw Exceptions::FILE_OPEN_ERR("Cannot open " + filePath + " file\n");
        } else {
            uint32_t hash = 0x00000000;
            size_t fileLength = fileSize(filePath);
            if (fileLength <= 0) {
                throw Exceptions::FILE_SIZE_ERR(filePath + " file is empty\n");
            }
            size_t buffSize = 32;            
            size_t readedBits = 0;
            char *buff = new char[buffSize];
            while (readedBits + buffSize < fileLength) {
                file.read(buff, buffSize);
                readedBits += buffSize;
                hash = CRC::crc32(buff, hash, buffSize);
            }
            delete buff;
            if (readedBits < fileLength) {
                buffSize = fileLength - readedBits;
                buff = new char[buffSize];
                file.read(buff, buffSize);
                hash = CRC::crc32(buff, hash, buffSize);
            }
            delete buff;

            if (hash != hashSum) {
                std::cerr   << "\nFile " << filePath << " was changed\n";
                std::cerr   << "Calculated hash is " << std::hex << hash
                            << " expected " << std::hex << hashSum << '\n';
                return false;
            } else {
                std::cout   << "\nFile " << filePath << " is correct\n";
                std::cout   << "Calculated hash is " << std::hex << hash
                            << " expected " << std::hex << hashSum << '\n';
                return true;
            }
        }
    }

    std::vector<JsonFile> parseJsonFile(const std::string& jsonFilePath) {
        std::vector<JsonFile> files;
        
        try {
            std::ifstream file(jsonFilePath);
            if (!file.is_open()) {
                throw Exceptions::FILE_OPEN_ERR("Cannot open file.");
            }
            
            nlohmann::json jsonData;
            file >> jsonData;
            
            if (!jsonData.contains("files") || !jsonData["files"].is_array()) {
                throw Exceptions::JSON_VAL_EXTRUCT_ERR("Invalid JSON format: Missing 'files' array.");
            }
            
            for (const auto& item : jsonData["files"]) {
                if (!item.contains("path") || !item.contains("hash")) {
                    std::cerr << KRED << "Missing file: Nesessary fields are empty.\n" << RST;
                    continue;
                }
                if (!item["path"].is_string() || !item["hash"].is_string()) {
                    std::cerr << KRED << "Missing file: Invalid field's data.\n" << RST;
                    continue;
                }

                JsonFile fileInfo;
                fileInfo.filePath = item["path"].get<std::string>();
                fileInfo.hashSum = std::stoul(item["hash"].get<std::string>(), nullptr, 16);               ;

                files.push_back(fileInfo);
            }
        } catch (const std::exception& e) {
            std::cerr << KRED << "Error in JSON parsing: " << e.what() << '\n' << RST;
        }

        return files;
    }

}

bool hashSumChecking(const std::string &jsonFilePath) {
    bool ret = true;
    try {
        std::vector<FilesFuncs::JsonFile> files = FilesFuncs::parseJsonFile(jsonFilePath);
        size_t size = files.size();
        size_t count = 0;
        for (std::vector<FilesFuncs::JsonFile>::iterator it = files.begin(); it != files.end(); ++it) {
            try {
                if (!FilesFuncs::checkCurrentFile(it->filePath, it->hashSum)) {
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

bool test() {
    std::cout << "\nTesting is started...\n";
    bool test_result = true;
    std::cout << FYEL("==============================================================\n");
    std::cout << "Testing crc32 algorythm (string 123456789):\n";
    {
        std::string testStr1 = "123";
        std::string testStr2 = "4567";
        std::string testStr3 = "89";
        std::string testStr = testStr1 + testStr2 + testStr3;
        uint32_t testTmpHash = CRC::crc32(testStr1.data(), uint32_t(0x00000000), testStr1.length());
        uint32_t testHalfHash = CRC::crc32(testStr2.data(), testTmpHash, testStr2.length());
        testHalfHash = CRC::crc32(testStr3.data(), testHalfHash, testStr3.length());
        uint32_t testSingleHash = CRC::crc32(testStr.data(), uint32_t(0x00000000), testStr.length());
        std::cout << "Half string-> " << std::hex << testHalfHash << '\n';
        std::cout << "Single string-> " << std::hex << testSingleHash << '\n';
        if (testSingleHash == testHalfHash) {
            std::cout << FGRN("Test passed") << '\n';
        } else {
            std::cout << FRED("Test failed") << '\n';
            test_result = false;
        }        
    }
    std::cout << FYEL("==============================================================\n");
    std::cout << "Testing crc32 for single file (config.json):\n";
    {
        try {
            uint32_t test_Hash = 0xf4c9c4c8;

            if (FilesFuncs::checkCurrentFile("config.json", test_Hash)) {
                std::cout << FGRN("Test passed") << '\n';
            } else {
                std::cerr << FRED ("Test failed") << '\n';
                test_result = false;
            }
        } catch (Exceptions::FILE_OPEN_ERR foEx) {
            std::cerr << KRED << foEx.what();
            std::cerr << "Cannot do this test\n" << RST;
        }
        
    }
    std::cout << FYEL("==============================================================\n");
    std::cout << "Testing crc32 for table of files (reading from testing table.json):\n";
    {
        try {
            if (hashSumChecking("table.json")) {
                std::cout << KGRN << "\nTest passed\n";
            } else {
                std::cerr << KRED << "\nTest failed\n";
                test_result = false;
            }
        } catch (Exceptions::FILE_OPEN_ERR ex) {
            std::cerr << KRED << ex.what();
            std::cout << "Cannot do this test\n" << RST;
        }
        
    }    
    std::cout << FYEL("==============================================================\n") << std::endl;
    return test_result;
}

int main(int argc, char **argv) {
    if (std::string(argv[1]) == "-test") {
        return test();
    }
    else  {
        try {
            if (hashSumChecking(argv[1])) {
                std::cout << KGRN << "\nAll files are correct\n" << RST << std::endl;
            } else {
                std::cout << KRED << "\nSome files were changed\n" << RST << std::endl;
            }
        } catch (Exceptions::FILE_OPEN_ERR foEx) {
            std::cerr << KRED << foEx.what() << '\n' << RST;
        }
        
        return 0;
    }    
}
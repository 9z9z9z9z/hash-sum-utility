#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>

#include "Exceptions.h"
#include "colors.h"


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

namespace JsonFuncs {
    struct JsonFile {
        std::string filePath;
        uint32_t hashSum;
        
        JsonFile(const std::string &rFilePath, uint32_t rHashSum) {
            filePath = rFilePath;
            hashSum = rHashSum;
        }
    };

    bool checkCurrentFile(const std::string &filePath, const uint32_t hashSum) {
        size_t currentBit = 0;
        std::ifstream file(filePath, std::ifstream::binary);
        if (!file.is_open()) {
            throw Exceptions::FILE_OPEN_ERR("Cannot open " + filePath + " file\n");
        } else {
            uint32_t hash = 0x00000000;
            file.seekg(0, file.end);
            size_t fileLength = file.tellg();
            file.seekg(0, file.beg);
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

    std::string extractValue(const std::string &line, const std::string &key) {
        size_t pos = line.find(key);
        if (pos == std::string::npos || line.data() == nullptr) {
            throw Exceptions::JSON_VAL_EXTRUCT_ERR("Cannot extract " + key + " from " + line + '\n');
        }
        pos += key.length();
        ++pos;
        pos = line.find(":", pos);
        if (pos == std::string::npos) {
            std::cerr << KRED << R"(Cannot find ":" in )" << line << '\n' << RST;
            throw Exceptions::JSON_IN_LINE_ERR("Error in json file in " + line + '\n');
        }
        ++pos;

        while (pos < line.size() && (line[pos] == ' ' || line[pos] == '"')) {
            ++pos;
        }

        size_t endPos = line.find_first_of(",", pos);
        if (endPos == std::string::npos) {
            endPos = line.size();
        }

        std::string value = line.substr(pos, endPos - pos);
        value.erase(remove(value.begin(), value.end(), '"'), value.end());
        return value;
    }

    std::vector<JsonFile> parseJsonFile(std::ifstream &file) {
        std::vector<JsonFile> jsonFiles;
        std::string line;
        std::stringstream ss;
        std::string path;
        uint32_t hash;
        while (std::getline(file, line)) {
            if (line.find("},") != std::string::npos || line.find("]") != std::string::npos) {
                jsonFiles.push_back(JsonFile(path, hash));
                path = "";
                hash = 0;
                ss << std::flush;
            }
            try {
                if (line.find(PATH) != std::string::npos) {
                    path = extractValue(line, PATH);
                }
                if (line.find(HASH) != std::string::npos) {
                    std::string val = extractValue(line, HASH);
                    if (!val.empty()) {
                        hash = static_cast<uint32_t>(std::stoul(val, nullptr, 16));
                    }            
                }
            } catch (Exceptions::JSON_VAL_EXTRUCT_ERR jveEx) {
                std::cerr << KRED << jveEx.what() << RST;
                throw;
            } catch (Exceptions::JSON_IN_LINE_ERR jilEx) {
                std::cerr << KRED << jilEx.what() << RST;
                throw;
            }
            
        }
        return jsonFiles;
    }

}

bool hashSumChecking(const std::string &jsonFilePath) {
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) {
        throw Exceptions::FILE_OPEN_ERR("Cannot open " + jsonFilePath + '\n');
    } else {
        try {
            std::vector<JsonFuncs::JsonFile> files = JsonFuncs::parseJsonFile(file);
            size_t size = files.size();
            size_t count = 0;
            for (std::vector<JsonFuncs::JsonFile>::iterator it = files.begin(); it != files.end(); ++it) {
                try {
                    if (JsonFuncs::checkCurrentFile(it->filePath, it->hashSum)) {
                        ++count;
                    }
                } catch (...) {
                    --size;
                    continue;
                }
            }
            file.close();
            return count == size;
        } catch (...) {
            file.close();
            return false;
        }        
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

            if (JsonFuncs::checkCurrentFile("config.json", test_Hash)) {
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
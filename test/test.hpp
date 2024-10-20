#pragma once
#include "../stdafx.h"

#include "../crc32/crc.hpp"

const std::string DIRECTORY_PATH = "../test/";

bool test() {
    std::cout << "\nTesting is started...\n";
    bool test_result = true;
    std::cout << KYEL << "==============================================================\n" << RST;
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
            std::cout << KGRN << "Test passed\n" << RST;
        } else {
            std::cout << KRED << "Test failed\n" << RST;
            test_result = false;
        }        
    }
    std::cout << KYEL << "==============================================================\n" << RST;
    std::cout << "Testing crc32 for single file (config.json):\n";
    {
        try {
            uint32_t test_Hash = 0x6f635711;

            if (CRC::checkCurrentFile(DIRECTORY_PATH + "config.json", test_Hash)) {
                std::cout << KGRN << "\nTest passed\n" << RST;
            } else {
                std::cerr << KRED << "\nTest failed\n" << RST;
                test_result = false;
            }
        } catch (Exceptions::FILE_OPEN_ERR foEx) {
            std::cerr << KRED << foEx.what();
            std::cerr << "Cannot do this test\n" << RST;
        } catch (Exceptions::FILE_SIZE_ERR fsEx) {
            std::cerr << KRED << fsEx.what();
            std::cerr << "Cannot get file size\n" << RST;
        }
        
    }
    std::cout << KYEL << "==============================================================\n" << RST;
    std::cout << "Testing crc32 for table of files (reading from testing table.json):\n";
    {
        try {
            if (CRC::hashSumChecking(DIRECTORY_PATH + "table.json")) {
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
    std::cout << KYEL << "==============================================================\n" << RST;
    return test_result;
}
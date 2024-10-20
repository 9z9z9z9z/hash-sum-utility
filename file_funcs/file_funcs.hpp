#pragma once 
#include "../stdafx.h"

const static std::string PATH = "path";
const static std::string HASH = "hash";

namespace FilesFuncs {
    struct JsonFile {
        std::string filePath;
        uint32_t hashSum;
        JsonFile() {}
    };

    std::vector<FilesFuncs::JsonFile> parseJsonFile(const std::string& jsonFilePath) {
        std::vector<FilesFuncs::JsonFile> files;
        try {
            std::ifstream file(jsonFilePath);
            if (!file.is_open()) {
                throw Exceptions::FILE_OPEN_ERR("Cannot open file.");
            }

            nlohmann::json jsonData;
            file >> jsonData;
            
            if (!jsonData.contains("files") || !jsonData["files"].is_array()) {
                throw Exceptions::JSON_VAL_EXTRACT_ERR("Invalid JSON format: Missing 'files' array.");
            }
            
            for (const auto& item : jsonData["files"]) {
                if (!item.contains("path") || !item.contains("hash")) {
                    files.clear();
                    throw Exceptions::JSON_FORMAT_ERR("Error in json file: Necessary fields are empty.\n");
                }
                if (!item["path"].is_string() || !item["hash"].is_string()) {
                    files.clear();
                    throw Exceptions::JSON_FORMAT_ERR("Error in json file: Invalid field's data.\n");
                }

                FilesFuncs::JsonFile fileInfo;

                fileInfo.filePath = item["path"].get<std::string>();
                std::string buff;
                buff = item["hash"].get<std::string>();
                buff.erase(std::remove(buff.begin(), buff.end(), ' '), buff.end());
                fileInfo.hashSum = std::stoul(buff, nullptr, 16);

                files.push_back(fileInfo);
            }
        } catch (const std::exception& e) {
            std::cerr << KRED << "Error in JSON parsing: " << e.what() << '\n' << RST;
        }

        return files;
    }

}

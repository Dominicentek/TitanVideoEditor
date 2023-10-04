#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include <map>

std::vector<std::filesystem::path> list_files(std::filesystem::path path) {
    std::vector<std::filesystem::path> files = {};
    if (std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::vector<std::filesystem::path> files_inner = list_files(entry.path());
            for (int i = 0; i < files_inner.size(); i++) {
                files.push_back(files_inner[i]);
            }
        }
    }
    else files.push_back(std::filesystem::absolute(path));
    return files;
}

std::string hex_str(unsigned char x) {
    std::string chars = "0123456789ABCDEF";
    return "" + std::string(1, chars.at(x / 16)) + std::string(1, chars.at(x % 16));
}

std::string c_filename(std::string str) {
    for (int i = 0; i < str.length(); i++) {
        char character = str[i];
        if (character >= 'A' && character <= 'Z') continue;
        if (character >= 'a' && character <= 'z') continue;
        if (character >= '0' && character <= '9') continue;
        str[i] = '_';
    }
    return str;
}

int main() {
    std::string assetdata = "#ifndef AssetData_H\n#define AssetData_H\n\n// Generated automatically\n// Any changes to this file will get overwritten by the build system\n";
    std::filesystem::path assets = std::filesystem::path("assets");
    std::vector<std::filesystem::path> files = list_files(assets);
    for (std::filesystem::path file : files) {
        int size = std::filesystem::file_size(file);
        char* content = (char*)malloc(size);
        std::ifstream stream = std::ifstream(file, std::ios::binary);
        stream.read(content, size);
        stream.close();
        std::string id = c_filename(std::filesystem::relative(file, assets).string());
        assetdata += "\ninline unsigned char " + id + "[] = {";
        for (int i = 0; i < size; i++) {
            if (i % 16 == 0) assetdata += "\n    ";
            assetdata += "0x" + hex_str(content[i]) + ",";
        }
        assetdata += "\n};\n";
        free(content);
    }
    assetdata += "\n#endif";
    std::ofstream stream = std::ofstream(std::filesystem::path("src/assets/assetdata.hpp"), std::ios::binary);
    stream.write(assetdata.c_str(), assetdata.length());
    stream.close();
    return 0;
}
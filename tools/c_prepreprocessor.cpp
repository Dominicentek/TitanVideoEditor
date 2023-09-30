#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <map>
#include <regex>

std::string read_file(std::filesystem::path path) {
    std::ifstream stream = std::ifstream(path, std::ios::binary);
    int size = std::filesystem::file_size(path);
    char* data = (char*)malloc(size + 1);
    stream.read(data, size);
    data[size] = 0;
    std::string content = std::string(data);
    free(data);
    stream.close();
    return content;
}

void write_file(std::filesystem::path path, std::string content) {
    std::ofstream stream = std::ofstream(path, std::ios::binary);
    stream.write(content.c_str(), content.size());
    stream.close();
}

std::vector<std::filesystem::path> list_files(std::filesystem::path path) {
    std::vector<std::filesystem::path> files = {};
    if (std::filesystem::is_directory(path)) {
        for (auto file : std::filesystem::directory_iterator(path)) {
            std::vector<std::filesystem::path> directory = list_files(file);
            for (std::filesystem::path filepath : directory) {
                files.push_back(filepath);
            }
        }
    }
    else files.push_back(path);
    return files;
}

int main() {
    std::vector<std::filesystem::path> sources = list_files("src");
    std::regex regex = std::regex(R"(\##(.*?)\##)");
    for (std::filesystem::path file : sources) {
        std::string input = read_file(file);
        std::string output = "";
        std::smatch match;
        while (std::regex_search(input, match, regex)) {
            std::string filepath = match[1].str();
            bool in_string = filepath[0] == '$';
            if (in_string) filepath = filepath.substr(1, filepath.size() - 1);
            std::string filedata = read_file(filepath);
            if (in_string) {
                filedata = std::regex_replace(filedata, std::regex("\n"), "\\n");
                filedata = std::regex_replace(filedata, std::regex("\""), "\\\"");
            }
            int next = match.position() + match.length();
            output += input.substr(0, match.position()) + filedata;
            input = input.substr(next, input.size() - next);
        }
        output += input;
        write_file(file, output);
    }
}

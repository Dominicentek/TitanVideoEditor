#include <vector>
#include <string>

std::vector<std::string> split_string(char delimiter, std::string input) {
    std::vector<std::string> tokens = {};
    std::string token = "";
    for (int i = 0; i < input.size(); i++) {
        char character = input[i];
        if (character == '\r') continue;
        if (character == delimiter) {
            tokens.push_back(token);
            token = "";
        }
        else token += character;
    }
    tokens.push_back(token);
    return tokens;
}
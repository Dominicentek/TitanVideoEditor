#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>
#include <string>

#pragma pack(push, 1) // Ensure that structure packing is 1 byte
struct BMPHeader {
    unsigned short signature;
    unsigned int fileSize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int dataOffset;
    unsigned int headerSize;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int dataSize;
    int horizontalResolution;
    int verticalResolution;
    unsigned int colors;
    unsigned int importantColors;
};
#pragma pack(pop)

struct Image {
    int* pixels;
    int width;
    int height;
};

std::string hex(unsigned int num) {
    std::string hex = "";
    std::string digits = "0123456789ABCDEF";
    while (num > 0) {
        int digit = num & 0xF;
        hex = digits[digit] + hex;
        num >>= 4;
    }
    while (hex.length() < 8) {
        hex = "0" + hex;
    }
    return "0x" + hex;
}

Image read_image(std::ifstream* stream) {
    BMPHeader header;
    Image image;
    stream->read((char*)&header, sizeof(BMPHeader));
    image.width = header.width;
    image.height = header.height;
    image.pixels = (int*)malloc(4 * image.width * image.height);
    stream->read((char*)image.pixels, 4 * image.width * image.height);
    return image;
}

void write_string(std::ofstream* stream, const char* string) {
    int length = 0;
    while (string[length] != 0) length++;
    stream->write(string, length);
}

void write_string(std::ofstream* stream, std::string string) {
    write_string(stream, string.c_str());
}

void write_image_data(std::ofstream* stream, std::string name, Image* image) {
    write_string(stream, "inline unsigned int " + name + "[] = {");
    for (int y = 0; y < image->height; y++) {
        for (int x = 0; x < image->width; x++) {
            int i = y * image->width + x;
            if (i % 8 == 0) write_string(stream, "\n    ");
            write_string(stream, hex(image->pixels[i]) + ",");
        }
    }
    write_string(stream, "\n};\n\n");
}

std::map<std::string, std::string> system_cursors = {
    { "move", "SIZEALL" },
    { "move_horizontal", "SIZEWE" },
    { "move_vertical", "SIZENS" }
};

#define HEADER "\n\n// This file is automatically generated by ImageToCpp\n// during the build process.\n\n// Any modifications to this file will be\n// overwritten during building.\n\n"

int main() {
    std::map<std::string, Image> icons = {};
    std::map<std::string, Image> cursors = {};
    for (auto path : std::filesystem::directory_iterator(std::filesystem::path("images"))) {
        std::ifstream stream = std::ifstream(path.path());
        std::string filename = path.path().filename().string();
        std::string extension = path.path().extension().string();
        filename = filename.substr(0, filename.length() - extension.length());
        if (extension != ".bmp") continue;
        if (filename.rfind("cursor_", 0) == 0) cursors.insert({ filename, read_image(&stream) });
        else icons.insert({ "icon_" + filename, read_image(&stream) });
        stream.close();
    }
    std::ofstream images_h = std::ofstream("src/gui/lib/images.hpp");
    std::ofstream icons_h = std::ofstream("src/gui/lib/icons.hpp");
    std::ofstream cursors_h = std::ofstream("src/gui/lib/cursors.hpp");
    std::ofstream icons_c = std::ofstream("src/gui/lib/icons.cpp");
    std::ofstream cursors_c = std::ofstream("src/gui/lib/cursors.cpp");
    write_string(&images_h, "#ifndef Images_H\n#define Images_H" HEADER);
    write_string(&icons_h, "#ifndef Icons_H\n#define Icons_H\n\n#include <SDL2/SDL.h>" HEADER);
    write_string(&cursors_h, "#ifndef Cursors_H\n#define Cursors_H\n\n#include <SDL2/SDL.h>" HEADER "extern SDL_Cursor* cursor_default;\n");
    write_string(&icons_c, "#include \"icons.hpp\"\n#include \"images.hpp\"" HEADER);
    write_string(&cursors_c, "#include \"cursors.hpp\"\n#include \"images.hpp\"" HEADER "SDL_Cursor* cursor_default = nullptr;\n");
    for (auto entry : icons) {
        write_image_data(&images_h, entry.first + "_data", &entry.second);
        write_string(&icons_h, "extern SDL_Texture* " + entry.first + ";\n");
        write_string(&icons_c, "SDL_Texture* " + entry.first + " = nullptr;\n");
    }
    for (auto entry : system_cursors) {
        write_string(&cursors_h, "extern SDL_Cursor* cursor_" + entry.first + ";\n");
        write_string(&cursors_c, "SDL_Cursor* cursor_" + entry.first + " = nullptr;\n");
    }
    for (auto entry : cursors) {
        write_image_data(&images_h, entry.first + "_data", &entry.second);
        write_string(&cursors_h, "extern SDL_Cursor* " + entry.first + ";\n");
        write_string(&cursors_c, "SDL_Cursor* " + entry.first + " = nullptr;\n");
    }
    write_string(&icons_c, "\nSDL_Texture* create_texture(SDL_Renderer* renderer, unsigned int* data, int width, int height) {\n    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, 32, width * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);\n    return SDL_CreateTextureFromSurface(renderer, surface);\n}\n\nvoid init_icons(SDL_Renderer* renderer) {\n");
    write_string(&cursors_c, "\nSDL_Surface* create_surface(unsigned int* data, int width, int height) {\n    return SDL_CreateRGBSurfaceFrom(data, width, height, 32, width * 4, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);\n}\n\nvoid init_cursors() {\n    cursor_default = SDL_GetDefaultCursor();\n");
    for (auto entry : icons) {
        write_string(&icons_c, "   " + entry.first + " = create_texture(renderer, " + entry.first + "_data, " + std::to_string(entry.second.width) + ", " + std::to_string(entry.second.height) + ");\n");
    }
    for (auto entry : system_cursors) {
        write_string(&cursors_c, "    cursor_" + entry.first + " = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_" + entry.second + ");\n");
    }
    for (auto entry : cursors) {
        write_string(&cursors_c, "    " + entry.first + " = SDL_CreateColorCursor(create_surface(" + entry.first + "_data, " + std::to_string(entry.second.width) + ", " + std::to_string(entry.second.height) + "), " + std::to_string(entry.second.width / 2) + ", " + std::to_string(entry.second.height / 2) + ");\n");
    }
    write_string(&images_h, "#endif");
    write_string(&icons_h, "\nextern void init_icons(SDL_Renderer* renderer);\n\n#endif");
    write_string(&cursors_h, "\nextern void init_cursors();\n\n#endif");
    write_string(&icons_c, "}");
    write_string(&cursors_c, "}");
    images_h.close();
    icons_h.close();
    cursors_h.close();
    icons_c.close();
    cursors_c.close();
    return 0;
}
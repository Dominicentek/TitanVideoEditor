#include <vector>
#include <string>
#include <cstdio>
#include <SDL2/SDL.h>
#include <png.h>
#ifdef WINDOWS
#include <windows.h>
#endif

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


std::string exec_command(std::string command) {
    // popen opens up cmd window in windows smh, have to use this thing for windows
#ifdef WINDOWS // thanks integerbang
    HANDLE stdoutRead = INVALID_HANDLE_VALUE;
    HANDLE stdoutWrite = INVALID_HANDLE_VALUE;
    PROCESS_INFORMATION processInfo = {};
    char* cmd = strdup(command.c_str());
    std::string result;
    if (cmd) {
        SECURITY_ATTRIBUTES attributes = { .nLength = sizeof(SECURITY_ATTRIBUTES), .bInheritHandle = TRUE };
        if (CreatePipe(&stdoutRead, &stdoutWrite, &attributes, 0)) {
            if (SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0)) {
                STARTUPINFO startupInfo = { .cb = sizeof(STARTUPINFO), .dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW, .wShowWindow = SW_HIDE, .hStdOutput = stdoutWrite, .hStdError = stdoutWrite };
                if (CreateProcessA(nullptr, cmd, nullptr, nullptr, true, 0, nullptr, nullptr, &startupInfo, &processInfo)) {
                    CloseHandle(stdoutWrite);
                    stdoutWrite = INVALID_HANDLE_VALUE;
                    CloseHandle(processInfo.hThread);
                    DWORD bytesRead;
                    char buffer[4096];
                    while (ReadFile(stdoutRead, buffer, 4096, &bytesRead, nullptr) && bytesRead > 0) {
                        result.append(buffer, bytesRead);
                    }
                    WaitForSingleObject(processInfo.hProcess, INFINITE);
                    CloseHandle(processInfo.hProcess);
                }
            }
            CloseHandle(stdoutRead);
            if (stdoutWrite != INVALID_HANDLE_VALUE) CloseHandle(stdoutWrite);
        }
        free(cmd);
    }
    return result;
#else // this is why unix better
    FILE* cmd = popen(command.c_str(), "r");
    char buffer[4096];
    std::string out = "";
    while (!feof(cmd)) {
        if (fgets(buffer, 4096, cmd) != nullptr) {
            out += buffer;
        }
    }
    pclose(cmd);
    return out;
#endif
}

struct ReadPngInfo
{
    const uint8_t* const Buffer;
    png_size_t Read;
};

static void ReadPngData(png_structp png_ptr, png_bytep out, png_size_t readSize)
{
    auto ioPtr = static_cast<ReadPngInfo*>(png_get_io_ptr(png_ptr));
    memcpy(out, ioPtr->Buffer + ioPtr->Read, readSize);
    ioPtr->Read += readSize;
}

SDL_Surface* CreateSdlSurfaceFromPng(void* data)
{
    auto png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

    if (!png)
        abort();

    png_infop info = png_create_info_struct(png);
    if (!info)
    {
        png_destroy_read_struct(&png, nullptr, nullptr);
        abort();
    }

    if (setjmp(png_jmpbuf(png)))
    {
        png_destroy_info_struct(png, &info);
        png_destroy_read_struct(&png, nullptr, nullptr);
        abort();
    }

    ReadPngInfo readInfo{ static_cast<const uint8_t* const>(data), 0 };
    png_set_read_fn(png, &readInfo, ReadPngData);

    png_read_info(png, info);

    png_uint_32 width, height;
    int bitDepth;
    int colorType;

    if (!png_get_IHDR(png, info, &width, &height, &bitDepth, &colorType, nullptr, nullptr, nullptr))
    {
        png_destroy_info_struct(png, &info);
        png_destroy_read_struct(&png, nullptr, nullptr);
        abort();
    }

    const auto surface = SDL_CreateRGBSurface(0, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);

    auto pixels = (uint8_t*)surface->pixels;

    switch (colorType)
    {
    case PNG_COLOR_TYPE_RGBA:
    {
        auto bytesPerRow = png_get_rowbytes(png, info);
        auto rowData = new uint8_t[bytesPerRow];
        for (size_t y = 0; y < height; y++)
        {
            png_read_row(png, rowData, nullptr);

            for (size_t x = 0; x < width; x++)
            {
                auto red = rowData[x * 4];
                auto green = rowData[x * 4 + 1];
                auto blue = rowData[x * 4 + 2];
                auto alpha = rowData[x * 4 + 3];

                pixels[x * 4 + bytesPerRow * y] = red;
                pixels[x * 4 + bytesPerRow * y + 1] = green;
                pixels[x * 4 + bytesPerRow * y + 2] = blue;
                pixels[x * 4 + bytesPerRow * y + 3] = alpha;
            }
        }
        delete[] rowData;
        break;
    }
    default:
        printf("invalid color type: %d\n", colorType);
        exit(1);
    }

    png_destroy_info_struct(png, &info);
    png_destroy_read_struct(&png, nullptr, nullptr);
    return surface;
}

SDL_Texture* create_texture(SDL_Renderer* renderer, void* data) {
    const auto surface = CreateSdlSurfaceFromPng(data);
end:
    auto tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

float map(float x, float srcMin, float srcMax, float dstMin, float dstMax) {
    return (x - srcMin) / (srcMax - srcMin) * (dstMax - dstMin) + dstMin;
}

int min(int a, int b) {
    if (a < b) return a;
    return b;
}

int max(int a, int b) {
    if (a > b) return a;
    return b;
}

std::string format_string(std::string format, ...) {
    va_list args;
    char buf[1024];
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format.data(), args);
    va_end(args);
    return buf;
}
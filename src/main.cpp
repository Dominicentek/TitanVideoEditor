#include "main.hpp"

#include <SDL2/SDL.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <filesystem>
#include <fstream>

#include "gui/gui_layout.hpp"
#include "gui/lib/cursors.hpp"
#include "gui/lib/icons.hpp"
#include "gui/render_util.hpp"
#include "utils.hpp"

#include "libs/portable-file-dialogs.hpp"

int mouseX;
int mouseY;
int mouseScroll;

bool mousePressed;
bool mouseDown;
bool mousePrevDown;
bool rightMousePressed;
bool rightMouseDown;
bool rightMousePrevDown;

SDL_Window* currentWindow;
int windowWidth;
int windowHeight;

SDL_Cursor* next_cursor = nullptr;

std::vector<SDL_Keycode> heldKeys = {};
std::vector<SDL_Keycode> pressedKeys = {};

std::string tooltip = "";

bool is_key_pressed(SDL_Keycode code) {
    return std::find(pressedKeys.begin(), pressedKeys.end(), code) != pressedKeys.end();
}

bool is_key_held(SDL_Keycode code) {
    return std::find(heldKeys.begin(), heldKeys.end(), code) != heldKeys.end();
}

bool update() {
    tooltip = "";
    next_cursor = cursor_default;
    int mouseState = SDL_GetMouseState(&mouseX, &mouseY);
    mouseDown = mouseState & SDL_BUTTON_LMASK;
    mousePressed = !mousePrevDown && mouseDown;
    mousePrevDown = mouseDown;
    rightMouseDown = mouseState & SDL_BUTTON_RMASK;
    rightMousePressed = !rightMousePrevDown && rightMouseDown;
    rightMousePrevDown = rightMouseDown;
    SDL_GetWindowSize(currentWindow, &windowWidth, &windowHeight);
    SDL_Event event;
    mouseScroll = 0;
    pressedKeys.clear();
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) return true;
        if (event.type == SDL_MOUSEWHEEL) mouseScroll = -event.wheel.y;
        if (event.type == SDL_KEYDOWN) {
            SDL_Keycode keycode = event.key.keysym.sym;
            if (!is_key_held(keycode)) {
                heldKeys.push_back(keycode);
                pressedKeys.push_back(keycode);
            }
        }
        if (event.type == SDL_KEYUP) {
            SDL_Keycode keycode = event.key.keysym.sym;
            if (is_key_held(keycode)) heldKeys.erase(std::find(heldKeys.begin(), heldKeys.end(), keycode));
        }
    }
    return false;
}

void render(SDL_Renderer* renderer) {
    render_gui(renderer);
    if (tooltip != "") {
        int x = 8;
        int y = 8;
        int w = 8 + tooltip.length() * 7;
        int h = 8 + 14;
        if (mouseX > windowWidth - x - w - 8) x = x * -1 - w;
        if (mouseY > windowHeight - y - h - 8) y = y * -1 - h;
        render_rect(renderer, mouseX + x, mouseY + y, w, h, 0x0000007F);
        render_text(renderer, mouseX + x + 4, mouseY + y + 4, tooltip);
    }
}

bool check_ffmpeg() {
    std::string path = std::string(getenv("PATH"));
#ifdef WINDOWS
    char delimiter = ';';
    std::string suffix = ".exe";
#else
    char delimiter = ':';
    std::string suffix = "";
#endif
    std::vector<std::string> paths = split_string(delimiter, path);
    std::vector<std::string> dependencies = { "ffmpeg", "ffprobe", "ffplay" };
    for (std::string p : paths) {
        for (int i = 0; i < dependencies.size(); i++) {
            std::filesystem::path fsPath = std::filesystem::path(p) / (dependencies[i] + suffix);
            if (std::filesystem::exists(fsPath)) {
                dependencies.erase(dependencies.begin() + i);
                i--;
            }
        }
    }
    if (dependencies.size() == 0) return false;
    std::string missing = "";
    for (int i = 0; i < dependencies.size(); i++) {
        if (i != 0) missing += ", ";
        missing += dependencies[i];
    }
    pfd::message("Titan Video Editor", "Cannot find FFmpeg executables in PATH\n\nDependencies missing:\n" + missing, pfd::choice::ok, pfd::icon::error);
    return true;
}

int main(int argc, char** argv) {
    if (check_ffmpeg()) return 1;
    SDL_Window* window = SDL_CreateWindow("Titan Video Editor - Alpha", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
    SDL_SetWindowResizable(window, SDL_TRUE);
    currentWindow = window;
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);
    init_cursors();
    init_icons(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    while (true) {
        clock_t before = clock();
        if (update()) break;
        render(renderer);
        SDL_SetCursor(next_cursor);
        SDL_RenderPresent(renderer);
        clock_t after = clock();
        if (after - before < CLOCKS_PER_SEC / 60) std::this_thread::sleep_for(std::chrono::microseconds((int)(1000000 / 60.0f - (float)(after - before) / CLOCKS_PER_SEC * 1000000)));
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
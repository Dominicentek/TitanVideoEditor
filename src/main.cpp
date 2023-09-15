#include <SDL2/SDL.h>
#include <chrono>
#include <thread>

int mouseX;
int mouseY;

bool mousePressed;
bool mouseDown;
bool mousePrevDown;

bool update() {
    mouseDown = SDL_GetMouseState(&mouseX, &mouseY) & SDL_BUTTON_LMASK;
    mousePressed = !mousePrevDown && mouseDown;
    mousePrevDown = mouseDown;
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            return true;
        }
    }
    return false;
}
void render() {

}
int main(int argc, char** argv) {
    SDL_Window* window = SDL_CreateWindow("Titan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 854, 480, 0);
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);
    while (true) {
        clock_t before = clock();
        if (update()) break;
        render();
        clock_t after = clock();
        if (after - before < CLOCKS_PER_SEC / 60) std::this_thread::sleep_for(std::chrono::microseconds((int)(1000000 / 60.0f - (float)(after - before) / CLOCKS_PER_SEC * 1000000)));
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
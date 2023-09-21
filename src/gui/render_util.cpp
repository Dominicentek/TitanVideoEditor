#include "render_util.hpp"
#include "gui/lib/icons.hpp"

#include <iostream>

int translateX = 0;
int translateY = 0;

void render_translate(int x, int y) {
    translateX += x;
    translateY += y;
}

void reset_translation() {
    translateX = 0;
    translateY = 0;
}

void render_rect(SDL_Renderer* renderer, int x, int y, int width, int height, int rgba) {
    SDL_Rect rect;
    rect.x = x + translateX;
    rect.y = y + translateY;
    rect.w = width;
    rect.h = height;
    SDL_SetRenderDrawColor(renderer, (rgba >> 24) & 0xFF, (rgba >> 16) & 0xFF, (rgba >> 8) & 0xFF, rgba & 0xFF);
    SDL_RenderFillRect(renderer, &rect);
}

void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height) {
    SDL_Rect rect;
    rect.x = x + translateX;
    rect.y = y + translateY;
    rect.w = width;
    rect.h = height;
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void render_text(SDL_Renderer* renderer, int x, int y, std::string text) {
    for (int i = 0; i < text.size(); i++) {
        int character = (int)text[i];
        SDL_Rect src;
        src.x = character % 16 * 7;
        src.y = (character / 16 - 2) * 14;
        src.w = 7;
        src.h = 14;
        SDL_Rect dst;
        dst.x = translateX + x + i * 7;
        dst.y = translateY + y;
        dst.w = 7;
        dst.h = 14;
        SDL_RenderCopy(renderer, icon_font, &src, &dst);
    }
}
#ifndef RenderUtil_H
#define RenderUtil_H

#include <SDL2/SDL.h>

#include <string>

extern void render_translate(int x, int y);
extern void reset_translation();
extern void render_rect(SDL_Renderer* renderer, int x, int y, int width, int height, int rgba);
extern void render_texture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y, int width, int height);
extern void render_text(SDL_Renderer* renderer, int x, int y, std::string text);

#endif
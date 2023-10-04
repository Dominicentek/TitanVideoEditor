#ifndef Utils_H
#define Utils_H

#include <SDL2/SDL.h>

#include <vector>
#include <string>

std::vector<std::string> split_string(char delimiter, std::string input);
std::string exec_command(std::string command);
SDL_Surface* CreateSdlSurfaceFromPng(void* data);
SDL_Texture* create_texture(SDL_Renderer* renderer, void* data);

#endif

#ifndef Utils_H
#define Utils_H

#include <vector>
#include <string>

std::vector<std::string> split_string(char delimiter, std::string input);
std::string exec_command(std::string command);
SDL_Surface* CreateSdlSurfaceFromPng(void* data);

#endif

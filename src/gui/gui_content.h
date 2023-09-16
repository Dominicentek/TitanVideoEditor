#ifndef GuiContent_H
#define GuiContent_H

#include <SDL2/SDL.h>

extern void gui_content_properties(SDL_Renderer* renderer, int x, int y, int w, int h);
extern void gui_content_file_browser(SDL_Renderer* renderer, int x, int y, int w, int h);
extern void gui_content_player(SDL_Renderer* renderer, int x, int y, int w, int h);
extern void gui_content_timer(SDL_Renderer* renderer, int x, int y, int w, int h);
extern void gui_content_timeline(SDL_Renderer* renderer, int x, int y, int w, int h);

#endif
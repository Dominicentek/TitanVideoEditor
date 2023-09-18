#ifndef Main_H
#define Main_H

#include <SDL2/SDL.h>

extern int mouseX;
extern int mouseY;
extern bool mousePressed;
extern bool mouseDown;
extern int mouseScroll;

extern SDL_Window* currentWindow;
extern int windowWidth;
extern int windowHeight;

extern SDL_Cursor* next_cursor;

extern bool is_key_pressed(SDL_KeyCode code);
extern bool is_key_held(SDL_KeyCode code);

#endif
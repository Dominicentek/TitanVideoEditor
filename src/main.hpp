#ifndef Main_H
#define Main_H

#ifdef WINDOWS
#define NULLFILE " > NUL"
#else
#define NULLFILE " 1> /dev/null 2> /dev/null"
#endif

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

extern bool is_key_pressed(SDL_Keycode code);
extern bool is_key_held(SDL_Keycode code);

#endif
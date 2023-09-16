#ifndef Cursors_H
#define Cursors_H

#include <SDL2/SDL.h>

extern SDL_Cursor* cursor_default;
extern SDL_Cursor* cursor_move_horizontal;
extern SDL_Cursor* cursor_move_vertical;
extern SDL_Cursor* cursor_move;

extern void init_cursors();

#endif
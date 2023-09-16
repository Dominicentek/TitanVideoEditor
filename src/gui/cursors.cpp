#include "cursors.h"

SDL_Cursor* cursor_default = nullptr;
SDL_Cursor* cursor_move_horizontal = nullptr;
SDL_Cursor* cursor_move_vertical = nullptr;
SDL_Cursor* cursor_move = nullptr;

void init_cursors() {
    cursor_default = SDL_GetDefaultCursor();
    cursor_move_horizontal = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    cursor_move_vertical = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    cursor_move = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
}
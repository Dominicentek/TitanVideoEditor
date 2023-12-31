#ifndef GuiLayout_H
#define GuiLayout_H

#include <SDL2/SDL.h>

#include "gui_content.hpp"

#include <string>

#define CONST(value) new float(value)

typedef void (*GuiWindowRenderer)(SDL_Renderer*, int, int, int, int);

struct GuiLayoutSection {
    GuiWindowRenderer render;
    float* x1;
    float* y1;
    float* x2;
    float* y2;
};

struct GuiLayoutSplitter {
    float* pos;
    float* valueMin;
    float* valueMax;
    float* extendMin;
    float* extendMax;
    bool isVertical;
};

struct GuiPopup {
    GuiWindowRenderer render;
    int x;
    int y;
    int w;
    int h;
};

inline float main_splitter_pos = 0.5f;
inline float tool_splitter_pos = 0.3333f;
inline float player_splitter_pos = 0.6667f;
inline float timeline_splitter_pos = 0.2f;

inline GuiLayoutSection gui_layout[] = {
    { gui_content_properties, CONST(0.0f), CONST(0.0f), &tool_splitter_pos, &main_splitter_pos },
    { gui_content_file_browser, &tool_splitter_pos, CONST(0.0f), &player_splitter_pos, &main_splitter_pos },
    { gui_content_player, &player_splitter_pos, CONST(0.0f), CONST(1.0f), &main_splitter_pos },
    { gui_content_timer, CONST(0.0f), &main_splitter_pos, &timeline_splitter_pos, CONST(1.0f) },
    { gui_content_timeline, &timeline_splitter_pos, &main_splitter_pos, CONST(1.0f), CONST(1.0f) },
};

inline GuiLayoutSplitter gui_splitters[] = {
    { &tool_splitter_pos, CONST(0.0f), &player_splitter_pos, CONST(0.0f), &main_splitter_pos, false },
    { &player_splitter_pos, &tool_splitter_pos, CONST(1.0f), CONST(0.0f), &main_splitter_pos, false },
    { &timeline_splitter_pos, CONST(0.0f), CONST(1.0f), &main_splitter_pos, CONST(1.0f), false },
    { &main_splitter_pos, CONST(0.0f), CONST(1.0f), CONST(0.0f), CONST(1.0f), true }
};

extern GuiLayoutSplitter* grabbedSplitter;
extern GuiPopup* current_popup;

extern void render_gui(SDL_Renderer* renderer);
extern void button_tooltip(std::string btn_tooltip);
extern bool button_icon(SDL_Renderer* renderer, SDL_Texture* icon, int x, int y, int w, int h, int color);
extern bool button_icon(SDL_Renderer* renderer, SDL_Texture* icon, int x, int y, int w, int h, int color, bool locked);
extern void open_popup(int x, int y, int w, int h, GuiWindowRenderer renderer);
extern void close_popup();

#undef CONST

#endif
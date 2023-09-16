#include "gui_layout.h"
#include "main.h"
#include "render_util.h"
#include "cursors.h"

#include <vector>
#include <map>
#include <utility>
#include <iostream>

GuiLayoutSplitter* grabbedSplitter = nullptr;

void render_gui(SDL_Renderer* renderer) {
    render_rect(renderer, 0, 0, windowWidth, windowHeight, 0x101010FF);
    for (int i = 0; i < sizeof(gui_layout) / sizeof(GuiLayoutSection); i++) {
        GuiLayoutSection section = gui_layout[i];
        float x1 = *section.x1 * (windowWidth - 10);
        float y1 = *section.y1 * (windowHeight - 10);
        float x2 = *section.x2 * (windowWidth - 10);
        float y2 = *section.y2 * (windowHeight - 10);
        if (*section.x1 != 0 && *section.x1 != 1) x1 += 2.5f;
        if (*section.y1 != 0 && *section.y1 != 1) y1 += 2.5f;
        if (*section.x2 != 0 && *section.x2 != 1) x2 -= 2.5f;
        if (*section.y2 != 0 && *section.y2 != 1) y2 -= 2.5f;
        render_rect(renderer, (int)x1 + 5, (int)y1 + 5, (int)(x2 - x1), (int)(y2 - y1), 0x202020FF);
    }
    if (!mouseDown) grabbedSplitter = nullptr;
    if (grabbedSplitter != nullptr) {
        if (grabbedSplitter->isVertical) *grabbedSplitter->pos = (mouseY - 5) / (float)(windowHeight - 10);
        else *grabbedSplitter->pos = (mouseX - 5) / (float)(windowWidth - 10);
        next_cursor = grabbedSplitter->isVertical ? cursor_move_vertical : cursor_move_horizontal;
    }
    for (int i = 0; i < sizeof(gui_splitters) / sizeof(GuiLayoutSplitter); i++) {
        GuiLayoutSplitter splitter = gui_splitters[i];
        float limitMin, limitMax;
        int x, y, w, h;
        if (splitter.isVertical) {
            x = *splitter.extendMin * (windowWidth - 10) + 5;
            y = *splitter.pos * (windowHeight - 10) + 2.5f;
            w = *splitter.extendMax * (windowWidth - 10);
            h = 5;
            limitMin = *splitter.valueMin + 10.0f / (windowHeight - 10);
            limitMax = *splitter.valueMax - 10.0f / (windowHeight - 10);
        }
        else {
            x = *splitter.pos * (windowWidth - 10) + 2.5f;
            y = *splitter.extendMin * (windowHeight - 10) + 5;
            w = 5;
            h = *splitter.extendMax * (windowHeight - 10);
            limitMin = *splitter.valueMin + 10.0f / (windowWidth - 10);
            limitMax = *splitter.valueMax - 10.0f / (windowWidth - 10);
        }
        if (*splitter.pos < limitMin) *splitter.pos = limitMin;
        if (*splitter.pos > limitMax) *splitter.pos = limitMax;
        if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h) {
            if (mousePressed) grabbedSplitter = &gui_splitters[i];
            next_cursor = (splitter.isVertical ? cursor_move_vertical : cursor_move_horizontal);
        }
    }
}
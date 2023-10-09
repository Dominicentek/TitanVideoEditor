#include "gui_layout.hpp"
#include "main.hpp"
#include "render_util.hpp"
#include "gui/lib/cursors.hpp"
#include "gui/lib/icons.hpp"

#include <vector>
#include <map>
#include <utility>
#include <iostream>

GuiLayoutSplitter* grabbedSplitter = nullptr;
GuiPopup* current_popup = nullptr;

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
        SDL_Rect rect;
        rect.x = (int)x1 + 5;
        rect.y = (int)y1 + 5;
        rect.w = (int)(x2 - x1);
        rect.h = (int)(y2 - y1);
        SDL_RenderSetClipRect(renderer, &rect);
        render_translate(rect.x, rect.y);
        render_rect(renderer, 0, 0, rect.w, rect.h, 0x202020FF);
        section.render(renderer, rect.x, rect.y, rect.w, rect.h);
        reset_translation();
        SDL_RenderSetClipRect(renderer, nullptr);
    }
    if (current_popup != nullptr) {
        render_rect(renderer, current_popup->x - 1, current_popup->y - 1, current_popup->w + 2, current_popup->h + 2, 0x101010FF);
        SDL_Rect rect;
        rect.x = current_popup->x;
        rect.y = current_popup->y;
        rect.w = current_popup->w;
        rect.h = current_popup->h;
        SDL_RenderSetClipRect(renderer, &rect);
        render_translate(rect.x, rect.y);
        render_rect(renderer, 0, 0, rect.w, rect.h, 0x202020FF);
        current_popup->render(renderer, rect.x, rect.y, rect.w, rect.h);
        reset_translation();
        SDL_RenderSetClipRect(renderer, nullptr);
        if (mousePressed && (
            mouseX < current_popup->x ||
            mouseY < current_popup->y ||
            mouseX >= current_popup->x + current_popup->w ||
            mouseY >= current_popup->y + current_popup->h)) close_popup();
    }
    if (!mouseDown) grabbedSplitter = nullptr;
    if (grabbedSplitter != nullptr) {
        if (grabbedSplitter->isVertical) *grabbedSplitter->pos = (mouseY - 5) / (float)(windowHeight - 10);
        else *grabbedSplitter->pos = (mouseX - 5) / (float)(windowWidth - 10);
        next_cursor = grabbedSplitter->isVertical ? cursor_move_vertical : cursor_move_horizontal;
        float limitMin, limitMax;
        if (grabbedSplitter->isVertical) {
            limitMin = *grabbedSplitter->valueMin + 10.0f / (windowHeight - 10);
            limitMax = *grabbedSplitter->valueMax - 10.0f / (windowHeight - 10);
        }
        else {
            limitMin = *grabbedSplitter->valueMin + 10.0f / (windowWidth - 10);
            limitMax = *grabbedSplitter->valueMax - 10.0f / (windowWidth - 10);
        }
        if (*grabbedSplitter->pos < limitMin) *grabbedSplitter->pos = limitMin;
        if (*grabbedSplitter->pos > limitMax) *grabbedSplitter->pos = limitMax;
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

bool button_icon(SDL_Renderer* renderer, SDL_Texture* icon, int x, int y, int w, int h, int color) {
    return button_icon(renderer, icon, x, y, w, h, color, false);
}

bool button_icon(SDL_Renderer* renderer, SDL_Texture* icon, int x, int y, int w, int h, int color, bool locked) {
    SDL_Rect rect;
    rect.x = x - 2;
    rect.y = y - 2;
    rect.w = w + 4;
    rect.h = h + 4;
    int r = (color >> 24) & 0xFF;
    int g = (color >> 16) & 0xFF;
    int b = (color >> 8) & 0xFF;
    int a = color & 0xFF;
    bool clicked = false;
    if (mouseX >= x - 2 && mouseY >= y - 2 && mouseX < x + w + 4 && mouseY < y + h + 4 && !locked) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rect);
        if (mousePressed) {
            mousePressed = false;
            clicked = true;
        }
    }
    rect.x += 2;
    rect.y += 2;
    rect.w -= 4;
    rect.h -= 4;
    if (locked) SDL_SetTextureColorMod(icon, 127, 127, 127);
    SDL_RenderCopy(renderer, icon, nullptr, &rect);
    SDL_SetTextureColorMod(icon, 255, 255, 255);
    return clicked;
}

void open_popup(int x, int y, int w, int h, GuiWindowRenderer renderer) {
    if (current_popup != nullptr) close_popup();
    current_popup = (GuiPopup*)malloc(sizeof(GuiPopup));
    current_popup->x = x;
    current_popup->y = y;
    current_popup->w = w;
    current_popup->h = h;
    current_popup->render = renderer;
}

void close_popup() {
    if (current_popup == nullptr) return;
    free(current_popup);
    current_popup = nullptr;
}
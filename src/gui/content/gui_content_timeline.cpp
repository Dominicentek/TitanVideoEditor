#include "gui/render_util.h"
#include "editor/editor.h"
#include "main.h"
#include "gui/gui_layout.h"
#include "gui/gui_content.h"

#include <string>

int scale = 10;
int position = 0;
bool dragging_frame = false;

float map(float x, float srcMin, float srcMax, float dstMin, float dstMax) {
    return (x - srcMin) / (srcMax - srcMin) * (dstMax - dstMin) + dstMin;
}

float get_space_between_frames() {
    float spaceBetweenFrames = 4;
    if (scale < 10) spaceBetweenFrames = map(scale, 1, 10, 30, 4);
    else if (scale > 10) spaceBetweenFrames = map(scale, 10, 20, 4, 0.25f);
    return spaceBetweenFrames;
}

void gui_content_timeline(SDL_Renderer* renderer, int x, int y, int w, int h) {
    render_rect(renderer, 0, 0, w, 24, 0x181818FF);
    if (scale > 20) scale = 20;
    if (scale < 1) scale = 1;
    float spaceBetweenFrames = get_space_between_frames();
    int numFrames = 1 / spaceBetweenFrames * w;
    for (int i = timer_scroll; i < tracks.size(); i++) {
        Track track = tracks[i];
        for (int j = 0; j < track.clips.size(); j++) {
            Clip clip = track.clips[j];
            if (clip.pos + clip.duration < position) continue;
            if (clip.pos > position + numFrames) continue;
            render_rect(renderer, (clip.pos - position) * spaceBetweenFrames, 24 + (i - timer_scroll) * 32 + 2, ceil(clip.duration * spaceBetweenFrames), 28, 0x404040FF);
            render_rect(renderer, (clip.pos - position) * spaceBetweenFrames + 1, 24 + (i - timer_scroll) * 32 + 3, ceil(clip.duration * spaceBetweenFrames) - 2, 26, 0x404040FF);
        }
        render_rect(renderer, 0, 24 + (i - timer_scroll) * 32 + 31, w, 2, 0x181818FF);
    }
    for (int i = 0; i < numFrames; i++) {
        int frame = position + i;
        if (frame == current_frame) {
            int posX = i * spaceBetweenFrames - 3;
            int posY = 3;
            render_rect(renderer, posX + 1, posY, 5, 5, 0xFF0000FF);
            render_rect(renderer, posX, posY + 1, 7, 2, 0xFF0000FF);
            render_rect(renderer, posX + 2, posY + 5, 3, 2, 0xFF0000FF);
            render_rect(renderer, posX + 3, posY + 7, 1, h, 0xFFFFFFFF);
            continue;
        }
        if (scale == 20 && frame % 30 != 0) continue;
        if (scale > 17 && frame % 15 != 0) continue;
        if (scale > 10 && frame % 5 != 0) continue;
        if (frame % 30 == 0) render_rect(renderer, i * spaceBetweenFrames, 24 - 8, 2, 8, 0xFFFFFFFF);
        else if (frame % 15 == 0) render_rect(renderer, i * spaceBetweenFrames, 24 - 8, 1, 8, 0xFFFFFFFF);
        else if (frame % 5 == 0) render_rect(renderer, i * spaceBetweenFrames, 24 - 6, 1, 6, 0xFFFFFFFF);
        else render_rect(renderer, i * spaceBetweenFrames, 24 - 4, 1, 4, 0xFFFFFFFF);
        if (frame % 30 != 0) continue;
        if (scale > 16 && frame % (30 * 5) != 0) continue;
        render_text(renderer, i * spaceBetweenFrames, 3, std::to_string(frame / 30));
    }
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + 24 && mousePressed) dragging_frame = true;
    if (dragging_frame) current_frame = round(position + (mouseX - x) / spaceBetweenFrames);
    if (!mouseDown) dragging_frame = false;
    if (current_frame < 0) current_frame = 0;
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h) {
        if (is_key_held(SDLK_LSHIFT)) position += scale * 3 * mouseScroll;
        else if (mouseScroll != 0) {
            scale += mouseScroll;
            spaceBetweenFrames = get_space_between_frames();
            position = current_frame - (0.5f * w / spaceBetweenFrames);
        }
    }
    if (position < 0) position = 0;
}
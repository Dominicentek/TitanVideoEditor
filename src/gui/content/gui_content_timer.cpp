#include "gui/render_util.h"
#include "editor/editor.h"
#include "gui/gui_layout.h"
#include "main.h"
#include "gui/lib/icons.h"

#include <iostream>

int timer_scroll;

std::string pad(char character, int count, std::string str) {
    int padCount = count - str.size();
    std::string out = "";
    for (int i = 0; i < padCount; i++) {
        out += character;
    }
    return out + str;
}

void gui_content_timer(SDL_Renderer* renderer, int x, int y, int w, int h) {
    int hours = current_frame / 30 / 60 / 60;
    int minutes = current_frame / 30 / 60 % 60;
    int seconds = current_frame / 30 % 60;
    int frame = current_frame % 30;
    char buffer[16];
    snprintf(buffer, 16, "%i:%02i:%02i;%02i", hours, minutes, seconds, frame);
    buffer[15] = 0;
    std::string text = std::string(buffer);
    render_rect(renderer, 0, 0, w, 24, 0x303030FF);
    render_text(renderer, w - 4 - text.size() * 7, 7, text);
    if (button_icon(renderer, icon_video, x + 4, y + 4, 16, 16, 0x404040FF)) {
        Track track;
        track.clips = {};
        track.type = TRACKTYPE_VIDEO;
        tracks.push_back(track);
    }
    if (button_icon(renderer, icon_sound, x + 4 + 20 + 4, y + 4, 16, 16, 0x404040FF)) {
        Track track;
        track.clips = {};
        track.type = TRACKTYPE_AUDIO;
        tracks.push_back(track);
    }
    if (button_icon(renderer, timeline_locking ? icon_magnet : icon_no_magnet, x + 4 + 20 + 4 + 20 + 4, y + 4, 16, 16, 0x404040FF)) {
        timeline_locking = !timeline_locking;
    }
    for (int i = timer_scroll; i < tracks.size(); i++) {
        render_rect(renderer, 2, 24 + (i - timer_scroll) * 32 + 2, w - 4, 28, 0x303030FF);
        render_texture(renderer, tracks[i].type == TRACKTYPE_AUDIO ? icon_sound : icon_video, 9, 24 + (i - timer_scroll) * 32 + 7, 16, 16);
        if (button_icon(renderer, icon_remove, x + w - 9 - 20, y + 24 + (i - timer_scroll) * 32 + 7, 16, 16, 0x404040FF)) {
            tracks.erase(tracks.begin() + i);
            i--;
        }
        if (button_icon(renderer, icon_up, x + w - 9 - 20 - 4 - 20, y + 24 + (i - timer_scroll) * 32 + 7, 16, 16, 0x404040FF, i == 0)) {
            Track temp = tracks[i];
            tracks[i] = tracks[i - 1];
            tracks[i - 1] = temp;
        }
        if (button_icon(renderer, icon_down, x + w - 9 - 20 - 4 - 20 - 4 - 20, y + 24 + (i - timer_scroll) * 32 + 7, 16, 16, 0x404040FF, i == tracks.size() - 1)) {
            Track temp = tracks[i];
            tracks[i] = tracks[i + 1];
            tracks[i + 1] = temp;
        }
    }
    int numTracks = (h - 24) / 32;
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h) timer_scroll += mouseScroll;
    if (timer_scroll > (int)tracks.size() - numTracks) timer_scroll = (int)tracks.size() - numTracks;
    if (timer_scroll < 0) timer_scroll = 0;
}
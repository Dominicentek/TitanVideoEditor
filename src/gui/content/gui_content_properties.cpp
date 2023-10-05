#include "gui/render_util.hpp"
#include "editor/editor.hpp"
#include "gui/gui_content.hpp"
#include "gui/lib/icons.hpp"
#include "main.hpp"
#include "gui/gui_layout.hpp"
#include "editor/filters.hpp"

#include <iostream>

int current_media_length = 0;
std::string current_media_name = "";
std::vector<TrackType> current_streams = {};
Filter* current_filter = nullptr;
Clip* current_clip = nullptr;
int current_clip_track_index = 0;
int current_clip_index = 0;
PropertiesMode properties_mode = PROPMODE_NONE_SELECTED;

std::string grabbed_media = "";
int grabbed_media_track_index = -1;
int grabbed_media_position = 0;
TrackType grabbed_media_type = TRACKTYPE_VIDEO;

int properties_scroll = 0;

bool mousePrevHeld = false;

int propmode_none_selected(SDL_Renderer* renderer, int x, int y, int w, int h) {
    std::string text = "No clip or file is selected";
    render_text(renderer, w / 2 - text.size() * 3.5f, 4, text);
    return 16;
}

int propmode_track_selector(SDL_Renderer* renderer, int x, int y, int w, int h) {
    int hours = current_media_length / 30 / 60 / 60;
    int minutes = current_media_length / 30 / 60 % 60;
    int seconds = current_media_length / 30 % 60;
    int frame = current_media_length % 30;
    char buffer[16];
    snprintf(buffer, 16, "%i:%02i:%02i;%02i", hours, minutes, seconds, frame);
    buffer[15] = 0;
    std::string text = std::string(buffer);
    for (int i = 0; i < current_streams.size(); i++) {
        int yPos = y + 24 + i * 32 - properties_scroll;
        if (mouseX >= x && mouseY >= yPos && mouseY >= y + 24 && mouseX < x + w && mouseY < yPos + 32 && mousePressed) {
            grabbed_media = current_media_name + "/" + (current_streams[i] == TRACKTYPE_VIDEO ? "v" : "a") + std::to_string(i);
            grabbed_media_type = current_streams[i];
        } 
        render_rect(renderer, 2, yPos - y, w - 4, 28, 0x303030FF);
        render_texture(renderer, current_streams[i] == TRACKTYPE_VIDEO ? icon_video : icon_sound, 8, 30 + i * 32 - properties_scroll, 16, 16);
        render_text(renderer, 32, 32 + i * 32 - properties_scroll, current_streams[i] == TRACKTYPE_VIDEO ? "Video Track" : "Audio Track");
    }
    if (mousePrevHeld && !mouseDown && grabbed_media_track_index != -1 && grabbed_media != "") {
        Clip clip;
        clip.media = grabbed_media;
        clip.duration = current_media_length;
        clip.speed = 1;
        clip.fade = 1;
        clip.trim = 0;
        clip.start_fade = 0;
        clip.end_fade = 0;
        clip.filters = {};
        clip.pos = grabbed_media_position;
        tracks[grabbed_media_track_index].clips.push_back(clip);
    }
    mousePrevHeld = mouseDown;
    if (!mouseDown) grabbed_media = "";
    render_rect(renderer, 0, 0, w, 24, 0x202020FF);
    render_text(renderer, 4, 7, current_media_name);
    render_text(renderer, w - 4 - text.size() * 7, 7, text);
    return 24 + current_streams.size() * 32;
}

int propmode_clip_settings(SDL_Renderer* renderer, int x, int y, int w, int h) {
    if (button_icon(renderer, icon_remove, x + 4, y + 4, 16, 16, 0x303030FF)) {
        tracks[current_clip_track_index].clips.erase(tracks[current_clip_track_index].clips.begin() + current_clip_index);
        properties_change_mode(PROPMODE_NONE_SELECTED);
    }
    render_text(renderer, 26, 7, "Delete");
    if (tracks[current_clip_track_index].type == TRACKTYPE_VIDEO) {
        render_text(renderer, 4, 28, "Filters");
        if (button_icon(renderer, icon_add, x + w - 20, y + 24, 16, 16, 0x303030FF)) {
            properties_change_mode(PROPMODE_FILTER_SELECT);
        }
        for (int i = 0; i < current_clip->filters.size(); i++) {
            render_rect(renderer, 2, i * 24 + 48, w - 4, 24, 0x303030FF);
            render_rect(renderer, 3, i * 24 + 49, w - 6, 22, 0x404040FF);
            render_text(renderer, 6, i * 24 + 54, current_clip->filters[i].name);
            if (button_icon(renderer, icon_remove, x + w - 20, y + i * 24 + 52, 16, 16, 0x505050FF)) {
                current_clip->filters.erase(current_clip->filters.begin() + i);
                i--;
            }
            if (button_icon(renderer, icon_edit, x + w - 46, y + i * 24 + 52, 16, 16, 0x505050FF)) {
                current_filter = &current_clip->filters[i];
                properties_change_mode(PROPMODE_TRACK_CONFIG);
            }
            if (button_icon(renderer, icon_up, x + w - 72, y + i * 24 + 52, 16, 16, 0x505050FF, i == 0)) {
                Filter temp = current_clip->filters[i];
                current_clip->filters[i] = current_clip->filters[i - 1];
                current_clip->filters[i - 1] = temp;
            }
            if (button_icon(renderer, icon_down, x + w - 96, y + i * 24 + 52, 16, 16, 0x505050FF, i == current_clip->filters.size() - 1)) {
                Filter temp = current_clip->filters[i];
                current_clip->filters[i] = current_clip->filters[i + 1];
                current_clip->filters[i + 1] = temp;
            }
        }
        return 46 + current_clip->filters.size() * 28;
    }
}

int propmode_filter_select(SDL_Renderer* renderer, int x, int y, int w, int h) {
    if (button_icon(renderer, icon_back, x + 4, y + 4, 16, 16, 0x303030FF)) {
        properties_change_mode(PROPMODE_CLIP_SETTINGS);
    }
    render_text(renderer, 26, 7, "Back");
    for (int i = 0; i < available_filters.size(); i++) {
        render_rect(renderer, 2, i * 24 + 24, w - 4, 24, 0x303030FF);
        render_rect(renderer, 3, i * 24 + 25, w - 6, 22, 0x404040FF);
        render_text(renderer, 6, i * 24 + 30, available_filters[i].name);
        if (button_icon(renderer, icon_add, x + w - 20, y + i * 24 + 28, 16, 16, 0x505050FF)) {
            current_clip->filters.push_back(available_filters[i]);
            properties_change_mode(PROPMODE_CLIP_SETTINGS);
        }
    }
    return 22 + current_clip->filters.size() * 28;
}

int propmode_filter_config(SDL_Renderer* renderer, int x, int y, int w, int h) {
    int height = 24;
    if (button_icon(renderer, icon_back, x + 4, y + 4, 16, 16, 0x303030FF)) {
        properties_change_mode(PROPMODE_CLIP_SETTINGS);
    }
    render_text(renderer, 26, 7, "Back");
    return height;
}

void gui_content_properties(SDL_Renderer* renderer, int x, int y, int w, int h) {
    int height = 0;
    switch (properties_mode) {
        case PROPMODE_NONE_SELECTED:
            height = propmode_none_selected(renderer, x, y, w, h);
            break;
        case PROPMODE_TRACK_SELECTOR:
            height = propmode_track_selector(renderer, x, y, w, h);
            break;
        case PROPMODE_CLIP_SETTINGS:
            height = propmode_clip_settings(renderer, x, y, w, h);
            break;
        case PROPMODE_FILTER_SELECT:
            height = propmode_filter_select(renderer, x, y, w, h);
            break;
        case PROPMODE_FILTER_CONFIG:
            height = propmode_filter_config(renderer, x, y, w, h);
            break;
    }
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h) properties_scroll += mouseScroll * 24;
    if (properties_scroll > height - h) properties_scroll = height - h;
    if (properties_scroll < 0) properties_scroll = 0;
}


void properties_change_mode(PropertiesMode mode) {
    properties_scroll = 0;
    properties_mode = mode;
}

PropertiesMode properties_current_mode() {
    return properties_mode;
}
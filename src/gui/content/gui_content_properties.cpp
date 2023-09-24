#include "gui/render_util.hpp"
#include "editor/editor.hpp"
#include "gui/gui_content.hpp"
#include "gui/lib/icons.hpp"
#include "main.hpp"

int current_media_length = 0;
std::string current_media_name = "";
std::vector<TrackType> current_streams = {};
std::vector<Filter*> current_filters = {};
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

void gui_content_properties(SDL_Renderer* renderer, int x, int y, int w, int h) {
    int height = 0;
    switch (properties_mode) {
        case PROPMODE_NONE_SELECTED:
            height = propmode_none_selected(renderer, x, y, w, h);
            break;
        case PROPMODE_TRACK_SELECTOR:
            height = propmode_track_selector(renderer, x, y, w, h);
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
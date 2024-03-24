#include "editor.hpp"
#include "gui/gui_content.hpp"
#include "main.hpp"
#include "utils.hpp"

#include <vector>
#include <filesystem>
#include <utility>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>

template <typename T> struct UndoState {
    UndoState<T>* next;
    UndoState<T>* prev;
    T curr;
    bool latest;
    bool has_selected_clip;
};

int current_frame = 0;
std::vector<Track> tracks = {};
UndoState<std::vector<Track>>* undo_state = nullptr;
bool timeline_locking = true;

bool can_undo() {
    if (undo_state) {
        if (undo_state->latest) return true;
        if (!undo_state->next) return true;
        if (undo_state->prev) return true;
    }
    return false;
}

bool can_redo() {
    if (!undo_state) return false;
    return undo_state->next != nullptr;
}

void undo() {
    if (!can_undo()) return;
    if (!undo_state->latest && undo_state->next == nullptr) {
        UndoState<std::vector<Track>>* latest = new UndoState<std::vector<Track>>();
        latest->latest = true;
        latest->next = nullptr;
        latest->prev = undo_state;
        latest->curr = tracks;
        undo_state->next = latest;
        tracks = undo_state->curr;
        if (!undo_state->has_selected_clip) properties_change_mode(PROPMODE_NONE_SELECTED);
        return;
    }
    undo_state = undo_state->prev;
    tracks = undo_state->curr;
    if (!undo_state->has_selected_clip) properties_change_mode(PROPMODE_NONE_SELECTED);
}

void redo() {
    if (!can_redo()) return;
    undo_state = undo_state->next;
    tracks = undo_state->curr;
}

void clean_history_from(UndoState<std::vector<Track>>* state) {
    if (!state) return;
    if (state->next) clean_history_from(state->next);
    delete state;
}

void undo_step() {
    UndoState<std::vector<Track>>* entry = new UndoState<std::vector<Track>>();
    entry->prev = nullptr;
    if (undo_state) {
        UndoState<std::vector<Track>>* curr = undo_state;
        bool has_latest = false;
        while (curr) {
            if (curr->latest) {
                has_latest = true;
                break;
            }
            curr = curr->next;
        }
        if (has_latest) {
            entry->prev = undo_state->prev;
            clean_history_from(undo_state->next);
        }
        else {
            entry->prev = undo_state;
            clean_history_from(undo_state->next);
        }
    }
    if (entry->prev) entry->prev->next = entry;
    entry->next = nullptr;
    entry->curr = tracks;
    entry->latest = false;
    entry->has_selected_clip = properties_is_in_clip_editing_mode();
    undo_state = entry;
}

std::pair<int, std::vector<TrackType>> get_media_streams_and_duration(std::filesystem::path path) {
    std::string cmd = "ffprobe -v error -show_entries stream=codec_type:format=duration -of default=nw=1:nk=1 \"" + path.string() + "\"";
    std::string out = exec_command(cmd);
    float duration = 0.0f;
    std::vector<TrackType> streams = {};
    std::vector<std::string> lines = split_string('\n', out);
    for (std::string line : lines) {
        if (line == "video") streams.push_back(TRACKTYPE_VIDEO);
        else if (line == "audio") streams.push_back(TRACKTYPE_AUDIO);
        else if (line == "N/A") duration = 5;
        else {
            try {
                duration = std::stof(line);
            }
            catch (...) {}
        }
    }
    return { (int)(duration * 30), streams };
}

std::string generate_filter(Clip* clip) {
    std::string shader = "varying vec2 v_texCoords;\nuniform sampler2D u_texture;\n\n";
    for (const Filter& filter : clip->filters) {
        shader += filter.shader + "\n";
    }
    shader += "void main() {\n    vec4 color = texture2D(u_texture, v_texCoords);\n";
    for (const Filter& filter : clip->filters) {
        shader += "    color = " + filter.mainFunc + "(color);\n";
    }
    shader += "    gl_FragColor = color;\n}";
    return shader;
}
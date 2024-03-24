#include "gui/render_util.hpp"
#include "editor/editor.hpp"
#include "main.hpp"
#include "gui/gui_layout.hpp"
#include "gui/gui_content.hpp"
#include "gui/lib/cursors.hpp"
#include "utils.hpp"

#include <string>
#include <iostream>

#define GRAB_TYPE_MOVE 0
#define GRAB_TYPE_LEFT_MOVE 1
#define GRAB_TYPE_RIGHT_MOVE 2
#define GRAB_TYPE_LEFT_FADE 3
#define GRAB_TYPE_RIGHT_FADE 4

int scale = 10;
int position = 0;
bool dragging_frame = false;

Clip* grabbedClip = nullptr;
int grabType = -1;
int clipGrabOffset = 0;
int grabbedClipTrackIndex = 0;
int grabbedClipIndex = 0;

std::vector<int> lock_positions = {};

float get_space_between_frames() {
    float spaceBetweenFrames = 4;
    if (scale < 10) spaceBetweenFrames = map(scale, 1, 10, 30, 4);
    else if (scale > 10) spaceBetweenFrames = map(scale, 10, 20, 4, 0.25f);
    return spaceBetweenFrames;
}

std::string get_clip_display_text(std::string raw, int clipWidth) {
    int maxChars = (clipWidth - 20) / 7;
    std::string display = "";
    for (int i = 0; i < raw.length() && i < maxChars; i++) {
        if (raw[i] == '/') break;
        display += raw[i];
    }
    return display;
}

int get_locking_position(int pos, int x) {
    if (!timeline_locking) return pos;
    float spaceBetweenFrames = get_space_between_frames();
    for (int i = 0; i < lock_positions.size(); i++) {
        int timelinePos = lock_positions[i] * spaceBetweenFrames;
        if (x >= timelinePos - 10 && x < timelinePos + 10) return lock_positions[i];
    }
    return pos;
}

void gui_content_timeline(SDL_Renderer* renderer, int x, int y, int w, int h) {
    render_rect(renderer, 0, 0, w, 24, 0x181818FF);
    float spaceBetweenFrames = get_space_between_frames();
    int numFrames = 1 / spaceBetweenFrames * w;
    grabbed_media_track_index = -1;
    int mouseFramePos = round(position + (mouseX - x) / spaceBetweenFrames);
    int mouseTrackPos = -1;
    lock_positions.clear();
    lock_positions.push_back(current_frame);
    grabbed_media_position = mouseFramePos;
    if (grabbed_media_position < 0) grabbed_media_position = 0;
    for (int i = timer_scroll; i < tracks.size(); i++) {
        Track track = tracks[i];
        for (int j = 0; j < track.clips.size(); j++) {
            Clip clip = track.clips[j];
            if (clip.pos + clip.duration < position) continue;
            if (clip.pos > position + numFrames) continue;
            int clipX = (clip.pos - position) * spaceBetweenFrames;
            int clipY = 24 + (i - timer_scroll) * 32 + 2;
            int clipW = ceil(clip.duration * spaceBetweenFrames);
            int clipH = 28;
            int clipColor = 0x404040FF;
            int clipGrabColor = 0x303030FF;
            if (current_clip_track_index == i && current_clip_index == j && properties_is_in_clip_editing_mode()) {
                clipColor = 0x606060FF;
                clipGrabColor = 0x505050FF;
            }
            int grabW = std::max(0, std::min(clipW, 5));
            int moveW = std::max(0, clipW - 10);
            int nextGrabType = -1;
            if (mouseX >= x + clipX && mouseY >= y + clipY && mouseX < x + clipX + grabW && mouseY < y + clipY + clipH) nextGrabType = GRAB_TYPE_LEFT_MOVE;
            if (mouseX >= x + clipX + clipW - grabW && mouseY >= y + clipY && mouseX < x + clipX + clipW && mouseY < y + clipY + clipH) nextGrabType = GRAB_TYPE_RIGHT_MOVE;
            if (mouseX >= x + clipX + grabW && mouseY >= y + clipY && mouseX < x + clipX + clipW - grabW && mouseY < y + clipY + clipH) nextGrabType = GRAB_TYPE_MOVE;
            if (nextGrabType == GRAB_TYPE_LEFT_MOVE) next_cursor = cursor_clip_left;
            if (nextGrabType == GRAB_TYPE_RIGHT_MOVE) next_cursor = cursor_clip_right;
            if (nextGrabType == GRAB_TYPE_MOVE) next_cursor = cursor_move;
            if (mousePressed && nextGrabType != -1) {
                grabbedClipIndex = j;
                grabbedClipTrackIndex = i;
                grabbedClip = &tracks[i].clips[j];
                grabType = nextGrabType;
                clipGrabOffset = mouseFramePos - clip.pos;
                undo_step();
            }
            if (rightMousePressed && mouseX >= x + clipX && mouseY >= y + clipY && mouseX < x + clipX + clipW && mouseY < y + clipY + clipH) {
                current_clip = &tracks[i].clips[j];
                current_clip_index = j;
                current_clip_track_index = i;
                properties_change_mode(PROPMODE_CLIP_SETTINGS);
            }
            render_rect(renderer, clipX, clipY, clipW, clipH, clipColor);
            render_rect(renderer, clipX, clipY, grabW, clipH, clipGrabColor);
            render_rect(renderer, clipX + clipW - grabW, clipY, grabW, clipH, clipGrabColor);
            render_text(renderer, clipX + 10, clipY + 8, get_clip_display_text(clip.media, clipW));
            if (grabbedClipIndex == j && grabbedClipTrackIndex == i) continue;
            lock_positions.push_back(clip.pos);
            lock_positions.push_back(clip.pos + clip.duration);
        }
        render_rect(renderer, 0, 24 + (i - timer_scroll) * 32 + 31, w, 2, 0x181818FF);
        if (mouseX >= x && mouseY >= y + 24 + (i - timer_scroll) * 32 && mouseY >= y + 24 && mouseX < x + w && mouseY < y + 24 + (i - timer_scroll) * 32 + 32) {
            if (grabbed_media_type == tracks[i].type) {
                bool unobstructed = true;
                for (const auto& c : tracks[i].clips) {
                    if (max(c.pos, grabbed_media_position) <= min(c.pos + c.duration, grabbed_media_position + current_media_length)) {
                        unobstructed = false;
                        break;
                    }
                }
                if (unobstructed) grabbed_media_track_index = i;
            }
            mouseTrackPos = i;
        }
        if (grabbed_media_track_index == i && mouseDown && grabbed_media != "") {
            render_rect(renderer, (grabbed_media_position - position) * spaceBetweenFrames, 24 + (i - timer_scroll) * 32 + 2, ceil(current_media_length * spaceBetweenFrames), 28, 0x4040407F);
        }
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
    if (dragging_frame) current_frame = mouseFramePos;
    if (current_frame < 0) current_frame = 0;
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h && grabbedClip == nullptr) {
        if (is_key_held(SDLK_LSHIFT)) position += scale * 5 * mouseScroll;
        else if (mouseScroll != 0) {
            scale += mouseScroll;
            if (scale > 20) scale = 20;
            if (scale < 1) scale = 1;
            spaceBetweenFrames = get_space_between_frames();
            position = current_frame - (0.5f * w / spaceBetweenFrames);
        }
    }
    if (position < 0) position = 0;
    if (mouseTrackPos == -1) {
        if (mouseY < windowHeight * main_splitter_pos + 32) mouseTrackPos = 0;
        else mouseTrackPos = tracks.size() - 1;
    }
    if (grabbedClip != nullptr) {
        if (grabType == GRAB_TYPE_LEFT_MOVE) {
            next_cursor = cursor_clip_left;
            int prev = grabbedClip->pos;
            grabbedClip->pos = get_locking_position(mouseFramePos, mouseX - x);
            grabbedClip->trim += grabbedClip->pos - prev;
            if (grabbedClip->duration < 1) {
                grabbedClip->pos -= -grabbedClip->duration + 1;
                grabbedClip->trim += -grabbedClip->duration + 1;
                grabbedClip->duration = 1;
            }
            Clip* obstructClip = nullptr;
            for (int i = 0; i < tracks[grabbedClipTrackIndex].clips.size(); i++) {
                Clip* c = &tracks[grabbedClipTrackIndex].clips[i];
                if (c == grabbedClip) continue;
                if (max(c->pos, grabbedClip->pos) <= min(c->pos + c->duration, grabbedClip->pos + grabbedClip->duration)) {
                    obstructClip = c;
                    break;
                }
            }
            if (obstructClip != nullptr) grabbedClip->pos = obstructClip->pos + obstructClip->duration;
            if (grabbedClip->pos < 0) grabbedClip->pos = 0;
            grabbedClip->duration += prev - grabbedClip->pos;
        }
        if (grabType == GRAB_TYPE_RIGHT_MOVE) {
            next_cursor = cursor_clip_right;
            grabbedClip->duration = get_locking_position(mouseFramePos, mouseX - x) - grabbedClip->pos;
            if (grabbedClip->duration < 1) grabbedClip->duration = 1;
            Clip* obstructClip = nullptr;
            for (int i = 0; i < tracks[grabbedClipTrackIndex].clips.size(); i++) {
                Clip* c = &tracks[grabbedClipTrackIndex].clips[i];
                if (c == grabbedClip) continue;
                if (max(c->pos, grabbedClip->pos) <= min(c->pos + c->duration, grabbedClip->pos + grabbedClip->duration)) {
                    obstructClip = c;
                    break;
                }
            }
            if (obstructClip != nullptr) grabbedClip->duration = obstructClip->pos - grabbedClip->pos;
        }
        if (grabType == GRAB_TYPE_MOVE) {
            next_cursor = cursor_move;
            grabbedClip->pos = get_locking_position(mouseFramePos - clipGrabOffset, mouseX - clipGrabOffset * spaceBetweenFrames - x);
            grabbedClip->pos = get_locking_position(grabbedClip->pos + grabbedClip->duration, mouseX + (grabbedClip->duration - clipGrabOffset) * spaceBetweenFrames - x) - grabbedClip->duration;
            if (tracks[grabbedClipTrackIndex].type == tracks[mouseTrackPos].type && grabbedClipTrackIndex != mouseTrackPos) {
                Clip clip = *grabbedClip;
                bool unobstructed = true;
                for (const auto& c : tracks[mouseTrackPos].clips) {
                    if (max(c.pos, clip.pos) <= min(c.pos + c.duration, clip.pos + clip.duration)) {
                        unobstructed = false;
                        break;
                    }
                }
                if (unobstructed) {
                    int index = tracks[mouseTrackPos].clips.size();
                    tracks[grabbedClipTrackIndex].clips.erase(tracks[grabbedClipTrackIndex].clips.begin() + grabbedClipIndex);
                    tracks[mouseTrackPos].clips.push_back(clip);
                    grabbedClipTrackIndex = mouseTrackPos;
                    grabbedClipIndex = index;
                    grabbedClip = &tracks[mouseTrackPos].clips[index];
                }
            }
            if (grabbedClip->pos < 0) grabbedClip->pos = 0;
            Clip* obstructClip = nullptr;
            for (int i = 0; i < tracks[grabbedClipTrackIndex].clips.size(); i++) {
                Clip* c = &tracks[grabbedClipTrackIndex].clips[i];
                if (c == grabbedClip) continue;
                if (max(c->pos, grabbedClip->pos) <= min(c->pos + c->duration, grabbedClip->pos + grabbedClip->duration)) {
                    obstructClip = c;
                    break;
                }
            }
            if (obstructClip != nullptr) {
                bool left = grabbedClip->pos < obstructClip->pos;
                if (left) grabbedClip->pos = obstructClip->pos - grabbedClip->duration;
                if (!left || grabbedClip->pos < 0) grabbedClip->pos = obstructClip->pos + obstructClip->duration;
            }
        }
    }
    if (!mouseDown) {
        dragging_frame = false;
        grabbedClip = nullptr;
        grabType = -1;
    }
}
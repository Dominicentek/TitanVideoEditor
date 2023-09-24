#include "gui/render_util.hpp"
#include "editor/editor.hpp"
#include "main.hpp"
#include "gui/gui_layout.hpp"
#include "gui/gui_content.hpp"
#include "gui/lib/cursors.hpp"

#include <string>

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

float map(float x, float srcMin, float srcMax, float dstMin, float dstMax) {
    return (x - srcMin) / (srcMax - srcMin) * (dstMax - dstMin) + dstMin;
}

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

void gui_content_timeline(SDL_Renderer* renderer, int x, int y, int w, int h) {
    render_rect(renderer, 0, 0, w, 24, 0x181818FF);
    float spaceBetweenFrames = get_space_between_frames();
    int numFrames = 1 / spaceBetweenFrames * w;
    grabbed_media_track_index = -1;
    int mouseFramePos = round(position + (mouseX - x) / spaceBetweenFrames);
    int mouseTrackPos = -1;
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
            }
            render_rect(renderer, clipX, clipY, clipW, clipH, 0x404040FF);
            render_rect(renderer, clipX, clipY, grabW, clipH, 0x303030FF);
            render_rect(renderer, clipX + clipW - grabW, clipY, grabW, clipH, 0x303030FF);
            render_text(renderer, clipX + 10, clipY + 8, get_clip_display_text(clip.media, clipW));
        }
        render_rect(renderer, 0, 24 + (i - timer_scroll) * 32 + 31, w, 2, 0x181818FF);
        if (mouseX >= x && mouseY >= y + 24 + (i - timer_scroll) * 32 && mouseY >= y + 24 && mouseX < x + w && mouseY < y + 24 + (i - timer_scroll) * 32 + 32) {
            if (grabbed_media_type == tracks[i].type) grabbed_media_track_index = i;
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
    grabbed_media_position = mouseFramePos;
    if (grabbed_media_position < 0) grabbed_media_position = 0;
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + 24 && mousePressed) dragging_frame = true;
    if (dragging_frame) current_frame = mouseFramePos;
    if (current_frame < 0) current_frame = 0;
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h && grabbedClip == nullptr) {
        if (is_key_held(SDLK_LSHIFT)) position += scale * 3 * mouseScroll;
        else if (mouseScroll != 0) {
            scale += mouseScroll;
            if (scale > 20) scale = 20;
            if (scale < 1) scale = 1;
            spaceBetweenFrames = get_space_between_frames();
            position = current_frame - (0.5f * w / spaceBetweenFrames);
        }
    }
    if (position < 0) position = 0;
    if (grabbedClip != nullptr) {
        if (grabType == GRAB_TYPE_LEFT_MOVE) {
            next_cursor = cursor_clip_left;
            int prev = grabbedClip->pos;
            grabbedClip->pos = mouseFramePos;
            grabbedClip->trim += grabbedClip->pos - prev;
            grabbedClip->duration -= grabbedClip->pos - prev;
            if (grabbedClip->duration < 1) {
                grabbedClip->pos -= -grabbedClip->duration + 1;
                grabbedClip->trim += -grabbedClip->duration + 1;
                grabbedClip->duration = 1;
            }
        }
        if (grabType == GRAB_TYPE_RIGHT_MOVE) {
            next_cursor = cursor_clip_right;
            grabbedClip->duration = mouseFramePos - grabbedClip->pos;
            if (grabbedClip->duration < 1) grabbedClip->duration = 1;
        }
        if (grabType == GRAB_TYPE_MOVE) {
            next_cursor = cursor_move;
            grabbedClip->pos = mouseFramePos - clipGrabOffset;
            if (grabbedClip->pos < 0) grabbedClip->pos = 0;
            if (mouseTrackPos != -1) {
                if (tracks[grabbedClipTrackIndex].type == tracks[mouseTrackPos].type && grabbedClipTrackIndex != mouseTrackPos) {
                    Clip clip = *grabbedClip;
                    int index = tracks[mouseTrackPos].clips.size();
                    tracks[grabbedClipTrackIndex].clips.erase(tracks[grabbedClipTrackIndex].clips.begin() + grabbedClipIndex);
                    tracks[mouseTrackPos].clips.push_back(clip);
                    grabbedClipTrackIndex = mouseTrackPos;
                    grabbedClipIndex = index;
                    grabbedClip = &tracks[mouseTrackPos].clips[index];
                }
            }
        }
        if (grabbedClip->pos < 0) grabbedClip->pos = 0;
    }
    if (!mouseDown) {
        dragging_frame = false;
        grabbedClip = nullptr;
        grabType = -1;
    }
}
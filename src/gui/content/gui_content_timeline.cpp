#include "gui/render_util.h"
#include "editor/editor.h"
#include "main.h"

int scale = 5;
int position = 0;

void gui_content_timeline(SDL_Renderer* renderer, int x, int y, int w, int h) {
    render_rect(renderer, 0, 0, w, 24, 0x303030FF);
    position += mouseScroll * 10 / scale;
    for (int i = 0; i < tracks.size(); i++) {
        Track track = tracks[i];
        for (int j = 0; j < track.clips.size(); j++) {
            Clip clip = track.clips[j];
            //if (clip.)
        }
    }
}
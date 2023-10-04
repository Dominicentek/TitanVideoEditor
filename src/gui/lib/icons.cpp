#include "utils.hpp"
#include "assets/assetdata.hpp"
#include <SDL2/SDL.h>

// This file is automatically generated by the build system
// Any changes to this file will be overwritten during build

SDL_Texture* icon_pause = nullptr;
SDL_Texture* icon_video = nullptr;
SDL_Texture* icon_render = nullptr;
SDL_Texture* icon_down = nullptr;
SDL_Texture* icon_no_magnet = nullptr;
SDL_Texture* icon_folder = nullptr;
SDL_Texture* icon_sound = nullptr;
SDL_Texture* icon_font = nullptr;
SDL_Texture* icon_play = nullptr;
SDL_Texture* icon_add = nullptr;
SDL_Texture* icon_remove = nullptr;
SDL_Texture* icon_up_dir = nullptr;
SDL_Texture* icon_beginning = nullptr;
SDL_Texture* icon_file = nullptr;
SDL_Texture* icon_magnet = nullptr;
SDL_Texture* icon_up = nullptr;

void init_icons(SDL_Renderer* renderer) {
    icon_pause = create_texture(renderer, images_pause_png);
    icon_video = create_texture(renderer, images_video_png);
    icon_render = create_texture(renderer, images_render_png);
    icon_down = create_texture(renderer, images_down_png);
    icon_no_magnet = create_texture(renderer, images_no_magnet_png);
    icon_folder = create_texture(renderer, images_folder_png);
    icon_sound = create_texture(renderer, images_sound_png);
    icon_font = create_texture(renderer, images_font_png);
    icon_play = create_texture(renderer, images_play_png);
    icon_add = create_texture(renderer, images_add_png);
    icon_remove = create_texture(renderer, images_remove_png);
    icon_up_dir = create_texture(renderer, images_up_dir_png);
    icon_beginning = create_texture(renderer, images_beginning_png);
    icon_file = create_texture(renderer, images_file_png);
    icon_magnet = create_texture(renderer, images_magnet_png);
    icon_up = create_texture(renderer, images_up_png);
}

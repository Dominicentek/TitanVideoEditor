#include "gui/render_util.hpp"
#include "gui/lib/icons.hpp"
#include "main.hpp"
#include "gui/gui_content.hpp"

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>

std::vector<std::filesystem::path> get_path(std::filesystem::path path) {
    std::vector<std::filesystem::path> dirs = {};
    path = std::filesystem::absolute(path);
    std::filesystem::path prev_path;
    while (prev_path != path) {
        dirs.insert(dirs.begin(), std::filesystem::absolute(path));
        prev_path = path;
        path = path.parent_path();
    }
    return dirs;
}

std::vector<std::filesystem::path> dir_stack = {};
int file_scroll = 0;

void gui_content_file_browser(SDL_Renderer* renderer, int x, int y, int w, int h) {
    if (dir_stack.empty()) dir_stack = get_path(std::filesystem::current_path());
    std::vector<std::string> files = {};
    std::vector<std::string> dirs = {};
    if (dir_stack.size() > 1) dirs.push_back("../");
    for (auto file : std::filesystem::directory_iterator(dir_stack[dir_stack.size() - 1])) {
        if (!std::filesystem::is_directory(file)) continue;
        dirs.push_back(file.path().filename().string() + "/");
    }
    for (auto file : std::filesystem::directory_iterator(dir_stack[dir_stack.size() - 1])) {
        if (std::filesystem::is_directory(file)) continue;
        files.push_back(file.path().filename().string());
    }
    auto string_comparator = [](std::string a, std::string b) {
        if (a == "../") return true;
        if (b == "../") return false;
        return a < b;
    };
    std::sort(dirs.begin(), dirs.end(), string_comparator);
    std::sort(files.begin(), files.end(), string_comparator);
    std::reverse(dirs.begin(), dirs.end());
    for (std::string dir : dirs) {
        files.insert(files.begin(), dir);
    }
    int height = files.size() * 20 + 4;
    if (mouseX >= x && mouseY >= y && mouseX < x + w && mouseY < y + h) file_scroll += mouseScroll * 20;
    if (file_scroll > height - h) file_scroll = height - h;
    if (file_scroll < 0) file_scroll = 0;
    for (int i = file_scroll / 20; i < (file_scroll + h) / 20 + 1; i++) {
        if (i < 0 || i >= files.size()) continue;
        bool is_dir = files[i][files[i].size() - 1] == '/';
        SDL_Texture* tex = is_dir ? icon_folder : icon_file;
        if (files[i] == "../") tex = icon_up_dir;
        if (mouseX >= x && mouseY >= y + 2 + i * 20 - file_scroll && mouseX < x + w && mouseY < y + 2 + (i + 1) * 20 - file_scroll &&
            mouseY >= y && mouseY < y + h) {
            render_rect(renderer, 0, 2 + i * 20 - file_scroll, w, 20, 0x303030FF);
            if (mousePressed) {
                if (is_dir) {
                    if (files[i] == "../") dir_stack.pop_back();
                    else dir_stack.push_back(dir_stack[dir_stack.size() - 1] / files[i]);
                    file_scroll = 0;
                }
                else {
                    current_media_name = files[i];
                    properties_change_mode(PROPMODE_TRACK_SELECTOR);
                    auto metadata = get_media_streams_and_duration(dir_stack[dir_stack.size() - 1] / files[i]);
                    current_media_length = metadata.first;
                    current_streams = metadata.second;
                }
            }
        }
        render_texture(renderer, tex, 4, i * 20 + 4 - file_scroll, 16, 16);
        render_text(renderer, 24, 7 + i * 20 - file_scroll, files[i]);
    }
}
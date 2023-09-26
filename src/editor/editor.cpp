#include "editor.hpp"
#include "main.hpp"
#include "utils.hpp"

#include <vector>
#include <filesystem>
#include <utility>
#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>

int current_frame = 0;
std::vector<Track> tracks = {};
bool timeline_locking = true;

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
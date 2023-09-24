#ifndef Editor_H
#define Editor_H

#include <vector>
#include <string>
#include <utility>
#include <filesystem>

enum KeyframeType {
    KFTYPE_FLOAT,
    KFTYPE_INT
};

enum KeyframeCurve {
    KFCURVE_LINEAR,
    KFCURVE_SINE_IN,
    KFCURVE_SINE_OUT,
    FKCURVE_SINE_INOUT,
    KFCURVE_QUAD_IN,
    KFCURVE_QUAD_OUT,
    FKCURVE_QUAD_INOUT,
    KFCURVE_CUBIC_IN,
    KFCURVE_CUBIC_OUT,
    FKCURVE_CUBIC_INOUT,
    KFCURVE_WAIT,
};

struct Keyframe {
    float values[4];
    int pos;
    KeyframeType type;
    KeyframeCurve curve;
};

enum FilterPropertyType {
    FILTERPROP_FLOAT,
    FILTERPROP_INT,
    FILTERPROP_BOOL,
    FILTERPROP_POINT,
    FILTERPROP_COLOR,
};

struct FilterProperty {
    std::string label;
    float values[4];
    FilterPropertyType type;
};

struct Filter {
    std::string shader;
    FilterProperty properties[16];
    int numProperties;
    std::vector<Keyframe> keyframes;
};

class FilterBuilder {
    private:
    Filter filter;
    public:
    FilterBuilder() {
        filter.numProperties = 0;
    }
    FilterBuilder* set_shader(std::string shader) {
        filter.shader = shader;
        return this;
    }
    FilterBuilder* add_property(std::string label, FilterPropertyType type, float val1 = 0, float val2 = 0, float val3 = 0, float val4 = 0) {
        FilterProperty property;
        property.label = label;
        property.type = type;
        property.values[0] = val1;
        property.values[1] = val2;
        property.values[2] = val3;
        property.values[3] = val4;
        filter.properties[filter.numProperties] = property;
        filter.numProperties++;
        return this;
    }
    Filter get() {
        return filter;
    }
};

enum TrackType {
    TRACKTYPE_VIDEO,
    TRACKTYPE_AUDIO
};

struct Clip {
    std::string media;
    int start_fade;
    int end_fade;
    int pos;
    int duration;
    int trim;
    float fade;
    float speed;
    std::vector<Filter*> filters;
};

struct Track {
    std::vector<Clip> clips;
    TrackType type;
};

extern std::vector<Track> tracks;
extern int current_frame;
extern bool timeline_locking;

extern std::pair<int, std::vector<TrackType>> get_media_streams_and_duration(std::filesystem::path path);

#endif
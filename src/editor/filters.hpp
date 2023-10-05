#ifndef Filters_H
#define Filters_H

#include "editor.hpp"
#include "assets/assetdata.hpp"

#include <vector>

std::vector<Filter> available_filters = {
    FilterBuilder().set_shader(std::string((char*)filters_test_glsl, filters_test_glsl_length), "filter_test")
        ->with_name("Test")
        ->add_property("Color", "test_color", FILTERPROP_COLOR, 1.0f, 1.0f, 1.0f, 1.0f)
        ->get()
};

#endif
#ifndef Filters_H
#define Filters_H

#include "editor.hpp"
#include "assets/assetdata.hpp"

#include <vector>

#define precision(x) (float)(log10(x))

std::vector<Filter> available_filters = {
    FilterBuilder().set_shader(std::string((char*)filters_test_glsl, filters_test_glsl_length), "filter_test")
        ->with_name("Test")
        ->add_property("Float", "test_float", FILTERPROP_FLOAT, 0.0f, -2.0f, 2.0f)
        ->add_property("Int", "test_float", FILTERPROP_INT, 0.f, -200.f, 200.f)
        ->add_property("Bool", "test_bool", FILTERPROP_BOOL, 0.0f)
        ->add_property("Color", "test_color", FILTERPROP_COLOR, 1.0f, 1.0f, 1.0f, 1.0f)
        ->add_property("Point", "test_point", FILTERPROP_POINT, 0.0f, 0.0f)
        ->get()
};

#undef precision

#endif
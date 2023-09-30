cmake_minimum_required(VERSION 3.16)

project(titan)

set(SOURCES
	src/main.cpp
	src/utils.cpp
	src/editor/editor.cpp
	src/gui/gui_layout.cpp
	src/gui/render_util.cpp
	src/gui/content/gui_content_file_browser.cpp
	src/gui/content/gui_content_player.cpp
	src/gui/content/gui_content_properties.cpp
	src/gui/content/gui_content_timeline.cpp
	src/gui/content/gui_content_timer.cpp
	src/gui/lib/cursors.cpp
	src/gui/lib/icons.cpp
	src/gui/lib/images.cpp
)

add_executable(image_to_cpp tools/image_to_cpp.cpp)

add_executable(titan ${SOURCES})
target_include_directories(titan PRIVATE src)
target_link_libraries(titan SDL2 png)
target_compile_options(titan PRIVATE -Wall -g -I src)

add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/src/gui/lib/images.cpp
	COMMAND $<TARGET_FILE:image_to_cpp> ${CMAKE_SOURCE_DIR}/images ${CMAKE_SOURCE_DIR}/src/gui/lib
)

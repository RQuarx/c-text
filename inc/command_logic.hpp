#pragma once

#include <string>

#include "sdl_helper.hpp"
#include "editor.hpp"


namespace CommandLogic {
    auto Handle(std::string &cmd, EditorData *editor_data, AppData *app_data) -> bool;
} /* namespace CommandLogic */
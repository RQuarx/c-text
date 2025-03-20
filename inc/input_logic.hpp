#pragma once

#include "editor.hpp"


class
InputLogic
{
public:
    static auto Handle_Backspace(EditorData *editor_data, bool is_lctrl_pressed) -> bool;
    static auto Handle_Return(EditorData *editor_data) -> bool;

private:
    static void Handle_Ctrl_Backspace(EditorData *editor_data);
};
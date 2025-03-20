#pragma once

#include "editor.hpp"


class
CursorLogic
{
public:
    static auto Move_Cursor_Right(EditorData *editor_data, bool is_lctrl_pressed) -> bool;
    static auto Move_Cursor_Left(EditorData *editor_data, bool is_lctrl_pressed) -> bool;
    static auto Move_Cursor_Down(EditorData *editor_data, bool is_lctrl_pressed) -> bool;
    static auto Move_Cursor_Up(EditorData *editor_data, bool is_lctrl_pressed) -> bool;

    CursorLogic() = delete;

private:
    static void Ctrl_Cursor_Right(EditorData *editor_data);
    static void Ctrl_Cursor_Left(EditorData *editor_data);
};
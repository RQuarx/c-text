#include "../../inc/input.hpp"

using Input::Logic;


auto
Logic::Handle_Backspace(Editor::Data *editor_data, bool is_lctrl_pressed) -> bool
{
    Position *cursor = &editor_data->cursor;

    if (cursor->Is_Zero()) return false;

    if (is_lctrl_pressed) {
        Handle_Ctrl_Backspace(editor_data);
        return true;
    }

    if (cursor->x <= 0 && cursor->y > 0) {
        size_t moved_line_len = editor_data->file_content.at(cursor->y).length();

        editor_data->file_content.at(cursor->y - 1)
            .append(editor_data->file_content.at(cursor->y));
        editor_data->file_content.erase(
            editor_data->file_content.begin() + cursor->y
        );

        cursor->y--;
        cursor->x =
            editor_data->file_content.at(cursor->y).length() - moved_line_len;
        return true;
    }

    editor_data->file_content.at(cursor->y).erase(cursor->x - 1, 1);
    cursor->x--;
    return true;
}


void
Logic::Handle_Ctrl_Backspace(Editor::Data *editor_data)
{
    Position *cursor = &editor_data->cursor;

    while (
        cursor->x > 0 &&
        Utils::Is_Word_Bound(
            editor_data->file_content.at(cursor->y).at(cursor->x - 1)
        )
    ) {
        editor_data->file_content.at(cursor->y).erase(cursor->x - 1, 1);
        cursor->x--;
    }

    while (
        cursor->x > 0 &&
        !Utils::Is_Word_Bound(
            editor_data->file_content.at(cursor->y).at(cursor->x - 1)
        )
    ) {
        editor_data->file_content.at(cursor->y).erase(cursor->x - 1, 1);
        cursor->x--;
    }

    editor_data->cursor_max_x = cursor->x;
}


auto
Logic::Handle_Return(Editor::Data *editor_data) -> bool
{
    std::string_view line = editor_data->file_content.at(editor_data->cursor.y);
    std::string insert_line;

    if (!line.empty() || !Utils::Is_All_Space(line)) {
        insert_line = line.substr(editor_data->cursor.x);
        editor_data->file_content.at(editor_data->cursor.y).erase(editor_data->cursor.x);
    } else {
        insert_line = "";
    }

    editor_data->file_content.insert(
        editor_data->file_content.begin() + editor_data->cursor.y + 1,
        insert_line
    );
    editor_data->cursor.y++;
    editor_data->cursor.x = 0;
    return true;
}
#include "../../inc/editor.hpp"
#include "../../inc/cursor.hpp"

using Cursor::Logic;


auto
Logic::Move_Cursor_Right(Editor::Data *editor_data, bool is_lctrl_pressed) -> bool
{
    int64_t line_len = editor_data->file_content.at(editor_data->cursor.y).length();
    if (editor_data->mode == Editor::Normal && line_len > 0) line_len--;

    Position *cursor = &editor_data->cursor;

    if (cursor->x < line_len) {
        if (is_lctrl_pressed) {
            Ctrl_Cursor_Right(editor_data);
        } else {
            cursor->x++;
            editor_data->cursor_max_x = cursor->x;
        }
        return true;
    }

    if (cursor->x >= line_len) {
        int64_t file_size = editor_data->file_content.size() - 1;
        if (cursor->y < file_size) {
            cursor->y++;
            cursor->x = 0;
        } else if (cursor->y == file_size && editor_data->mode == Editor::Normal) {
            cursor->x = line_len;
            return false;
        }
        return true;
    }
    return false;
}


void
Logic::Ctrl_Cursor_Right(Editor::Data *editor_data)
{
    std::string line = editor_data->file_content.at(editor_data->cursor.y);
    int64_t line_len = line.length();

    if (editor_data->mode == Editor::Normal && line_len > 0) line_len--;
    if (
        editor_data->cursor.x <= line_len &&
        Utils::Is_Word_Bound(line.at(editor_data->cursor.x))
    ) editor_data->cursor.x++;

    while (
        editor_data->cursor.x < line_len &&
        !Utils::Is_Word_Bound(line.at(editor_data->cursor.x))
    ) {
        editor_data->cursor.x++;
    }
    editor_data->cursor_max_x = editor_data->cursor.x;
}


auto
Logic::Move_Cursor_Left(Editor::Data *editor_data, bool is_lctrl_pressed) -> bool
{
    Position *cursor = &editor_data->cursor;
    uint8_t position_offset = (editor_data->mode == Editor::Normal ? 1 : 0);

    if (cursor->x > 0) {
        if (is_lctrl_pressed) {
            Ctrl_Cursor_Left(editor_data);
        } else {
            cursor->x--;
            editor_data->cursor_max_x = cursor->x;
        }
        return true;
    }

    if (cursor->y > 0 && cursor->x <= 0) {
        int64_t len = editor_data->file_content.at(cursor->y - 1).length() - position_offset;
        cursor->y--;
        cursor->x = std::max(len, 0L);
        editor_data->cursor_max_x = cursor->x;
        return true;
    }
    return false;
}


void
Logic::Ctrl_Cursor_Left(Editor::Data *editor_data)
{
    Position *cursor = &editor_data->cursor;
    std::string line = editor_data->file_content.at(cursor->y);
    uint8_t position_offset = (editor_data->mode == Editor::Normal ? 1 : 0);

    if (
        cursor->x > 0 &&
        Utils::Is_Word_Bound(line.at(editor_data->cursor.x - position_offset))
    ) cursor->x--;

    while (
        cursor->x > 0 &&
        !Utils::Is_Word_Bound(line.at(editor_data->cursor.x - position_offset))
    ) cursor->x--;
    editor_data->cursor_max_x = cursor->x;
}


auto
Logic::Move_Cursor_Down(Editor::Data *editor_data, bool is_lctrl_pressed) -> bool
{
    Position *cursor = &editor_data->cursor;

    if (cursor->y >= editor_data->file_content.size() - 1) { return false; }
    if (is_lctrl_pressed && editor_data->scroll.y < editor_data->file_content.size()) {
        editor_data->scroll.y++;
        return true;
    }

    if (++cursor->y >= editor_data->last_rendered_line) {
        while (cursor->y >= editor_data->last_rendered_line) {
            editor_data->last_rendered_line++;
            editor_data->scroll.y++;
        }
    }
    editor_data->scroll.y = std::min(cursor->y, editor_data->scroll.y);

    int64_t line_len =
        editor_data->file_content.at(cursor->y).length();
    if (editor_data->mode == Editor::Normal && line_len > 0) line_len--;

    cursor->x = editor_data->cursor_max_x;
    cursor->x = std::min(cursor->x, line_len);
    return true;
}


auto
Logic::Move_Cursor_Up(Editor::Data *editor_data, bool is_lctrl_pressed) -> bool
{
    Position *cursor = &editor_data->cursor;

    if (is_lctrl_pressed) {
        if (editor_data->scroll.y > 0) {
            editor_data->scroll.y--;
            return true;
        }
        return false;
    }

    if (cursor->y <= 0) return false;
    editor_data->scroll.y = std::min(--cursor->y, editor_data->scroll.y);

    int64_t line_len = editor_data->file_content.at(cursor->y).length();
    if (editor_data->mode == Editor::Normal && line_len > 0) { line_len--; }

    cursor->x = editor_data->cursor_max_x;
    cursor->x = std::min(cursor->x, line_len);
    return true;
}
#pragma once

#include "command_handler.hpp"
#include "cursor_renderer.hpp"
#include "sdl_helper.hpp"


enum EditorMode : uint8_t {
    Normal,
    Insert,
    Command,
    Visual,
    Replace
};

struct EditorData {
    std::vector<std::string> file_content;
    std::filesystem::path file_path;

    size_t last_rendered_line = 0;
    size_t max_editor_width = 0;

    Position position;
    Position scroll;
    Position cursor;
    int64_t cursor_max_x = 0;

    std::unordered_map<std::string_view, Cache*> caches;

    EditorMode mode = Normal;

    EditorData(std::vector<std::string> &file, std::filesystem::path &_file_path) :
        file_content(file),
        file_path(_file_path) {}

    explicit
    EditorData(std::filesystem::path &file_path) :
        file_path(file_path) {}
};


class
Editor
{
public:
    Editor(bool *return_code, std::string &file_path, CursorRenderer *cursor_renderer);

    Editor() = default;

    /// Initialises editor
    /// @returns true on success or false on failure.
    static auto
    Init_Editor(Editor *editor, std::string &file_path, CursorRenderer *cursor_renderer, bool debug) -> bool;

    /// Renders the editor
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    auto Render(AppData *app_data) -> bool;

    /// Returns the pointer to editor_data
    auto Get_Data() -> EditorData*;

private:
    std::unique_ptr<EditorData> editor_data;
    CursorRenderer *cursor_renderer{};

    auto Render_Line(
        AppData *app_data,
        Position render_position,
        int64_t current_index,
        int32_t line_height,
        int32_t *max_editor_width
    ) const -> bool;

    auto Render_Line_Number(
        AppData *app_data,
        int64_t line_index,
        Position position,
        int32_t *line_number_width
    ) const -> bool;

    auto Render_Text(
        AppData *app_data,
        Position position,
        int64_t line_index
    ) const -> bool;

    auto Render_Cursor(
        AppData *app_data,
        Position position,
        std::string &line
    ) const -> bool;
};
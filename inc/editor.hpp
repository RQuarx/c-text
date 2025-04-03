#pragma once

#include "sdl_helper.hpp"
#include "cursor.hpp"


namespace Editor {
    enum Mode : uint8_t {
        Command,
        Replace,
        Normal,
        Insert,
        Visual,
    };

    struct Data {
        std::vector<std::string> file_content;
        std::filesystem::path file_path;

        size_t last_rendered_line = 0;
        size_t max_editor_width = 0;

        Position position;
        Position scroll;
        Position cursor;
        int64_t cursor_max_x = 0;

        std::unordered_map<std::string_view, Cache*> caches;

        Mode mode = Normal;

        Data(std::vector<std::string> &file, std::filesystem::path &_file_path) :
            file_content(file),
            file_path(_file_path) {}

        explicit
        Data(std::filesystem::path &file_path) :
            file_path(file_path) {}
    };

    class
    UI
    {
    public:
        UI(bool *return_code, std::string &file_path, Cursor::Renderer *cursor_renderer);

        UI() = default;

        /// Initialises editor
        /// @returns true on success or false on failure.
        static auto
        Init(UI *editor_ui, std::string &file_path, Cursor::Renderer *cursor_renderer, bool debug) -> bool;

        /// Renders the editor
        /// @returns true on success or false on failure.
        /// @warning This function should only be called on the main thread.
        auto Render(AppData *app_data) -> bool;

        /// Returns the pointer to editor_data
        auto Get_Data() -> Data*;

    private:
        std::unique_ptr<Data> m_editor_data;
        Cursor::Renderer *m_cursor_renderer{};

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
} /* namespace Editor */
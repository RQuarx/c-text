#pragma once

#include "sdl_helper.hpp"

namespace Editor {
    struct Data;
};


namespace Cursor {
    enum Type : uint8_t {
        Underline,
        Beam,
        Box,
    };

    struct Data {
        Position cursor;
        Position line_offset;

        std::string line;
        Type type = Box;

        Data(std::string &line, Position line_offset, Position cursor, Type type) :
            cursor(cursor),
            line_offset(line_offset),
            line(line),
            type(type) {}
        Data() = default;
    };

    class
    Renderer
    {
    public:
        /// Renders a type of cursor based on the given data's
        /// @param data contains the renderer context. fpnt type, line, position, cursor position, and config context
        /// @returns true on success, or false on failure.
        /// @warning This function should only be called on the main thread.
        auto Render(AppData *app_data, Data *data, std::string_view font_type) -> bool;


    private:
        SDL_Renderer *m_renderer;
        Data *m_data;

        SDL_Color m_color;
        TTF_Font *m_font;
        int32_t m_width;

        /// Renders a beam cursor at the specified position
        /// @returns true on success, or false on failure.
        /// @warning This function should only be called on the main thread.
        auto Beam() -> bool;

        /// Renders a box cursor at the specified position
        /// @returns true on success, or false on failure.
        /// @warning This function should only be called on the main thread.
        auto Box() -> bool;

        /// Renders an underline cursor at the specified position
        /// @returns true on success, or false on failure.
        /// @warning This function should only be called on the main thread.
        auto Underline() -> bool;
    };

    class
    Logic
    {
    public:
        static auto Move_Cursor_Right(struct Editor::Data *editor_data, bool is_lctrl_pressed) -> bool;
        static auto Move_Cursor_Left(struct Editor::Data *editor_data, bool is_lctrl_pressed) -> bool;
        static auto Move_Cursor_Down(struct Editor::Data *editor_data, bool is_lctrl_pressed) -> bool;
        static auto Move_Cursor_Up(struct Editor::Data *editor_data, bool is_lctrl_pressed) -> bool;

        Logic() = delete;

    private:
        static void Ctrl_Cursor_Right(struct Editor::Data *editor_data);
        static void Ctrl_Cursor_Left(struct Editor::Data *editor_data);
    };
} /* namespace Cursor */
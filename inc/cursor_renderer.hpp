#pragma once

#include <utility>

#include "sdl_helper.hpp"


enum CursorType : uint8_t {
    Beam,
    Box,
    Underline
};


struct CursorData {
    std::string line;
    Position cursor;
    Position position;
    CursorType type = Box;
    std::string_view font_type;

    CursorData(std::string &line, Position cursor, Position pos, CursorType type, std::string_view font_type) :
        line(line),
        cursor(cursor),
        position(pos),
        type(type),
        font_type(font_type) {}

    CursorData() = default;
};


class
CursorRenderer
{
public:
    /// Renders a type of cursor based on the given data's
    /// @param data contains the renderer context. fpnt type, line, position, cursor position, and config context
    /// @returns true on success, or false on failure.
    /// @warning This function should only be called on the main thread.
    auto Render(AppData *app_data, CursorData *data) -> bool;


private:
    SDL_Renderer *renderer;
    std::string line;
    SDL_Color color;
    Position cursor;
    TTF_Font *font;
    int32_t width;
    Position pos;

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
    auto Under_Line() -> bool;
};
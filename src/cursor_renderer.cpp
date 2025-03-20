#include "../inc/cursor_renderer.hpp"


auto
CursorRenderer::Render(AppData *app_data, CursorData *data) -> bool
{
    color = app_data->config.Get_Color_Value("cursor", "color");
    width = app_data->config.Get_Int_Value("cursor", "width");
    renderer = app_data->renderer;
    cursor = data->cursor;
    pos = data->position;
    font = app_data->fonts.at(data->font_type);
    line = data->line;

    switch (data->type) {
    case CursorType::Beam:
        return Beam();
    case CursorType::Box:
        return Box();
    case CursorType::Underline:
        return Under_Line();
    default:
        return false;
    }
}


auto
CursorRenderer::Beam() -> bool
{
    std::string behind = line.substr(0, cursor.x);

    int32_t height = TTF_GetFontHeight(font);
    int32_t x = 0;

    if (cursor.x > 0) {
        SDL::Get_String_Size(font, behind, &x, nullptr);
    }

    SDL_FRect rect = {
        static_cast<float>(pos.x + x),
        static_cast<float>(pos.y + (cursor.y * height)),
        static_cast<float>(width),
        static_cast<float>(height)
    };

    return SDL::Draw_Rect_Outline({ renderer, color }, rect, { 0, 0, 0, width });
}


auto
CursorRenderer::Box() -> bool
{
    std::string behind = line.substr(0, cursor.x);
    char hovered_char = ' ';
    if (cursor.x < line.length()) {
        hovered_char = line.at(cursor.x);
    }

    int32_t height = 0;
    int32_t w = 0;
    int32_t x = 0;
    SDL::Get_Char_Size(font, hovered_char, &w, &height);

    if (cursor.x > 0) {
        SDL::Get_String_Size(font, behind, &x, nullptr);
    }

    SDL_FRect rect = {
        static_cast<float>(pos.x + x),
        static_cast<float>(pos.y + (cursor.y * height)),
        static_cast<float>(w),
        static_cast<float>(height)
    };

    return SDL::Draw_Rect_Outline({ renderer, color }, rect, { width, width, width, width });
}


auto
CursorRenderer::Under_Line() -> bool
{
    std::string behind = line.substr(0, cursor.x);
    char hovered_char = line.at(cursor.x);

    int32_t height = 0;
    int32_t width = 0;
    int32_t x = 0;
    SDL::Get_Char_Size(font, hovered_char, &width, &height);

    if (cursor.x > 0) {
        SDL::Get_String_Size(font, behind, &x, nullptr);
    }

    SDL_FRect rect = {
        static_cast<float>(pos.x + x),
        static_cast<float>(pos.y + (cursor.y * height)),
        static_cast<float>(width),
        static_cast<float>(height)
    };

    return SDL::Draw_Rect_Outline({ renderer, color }, rect, { 0, 0, width, 0 });
}
#include "../../inc/cursor.hpp"

using Cursor::Renderer;


auto
Renderer::Render(AppData *app_data, Cursor::Data *data, std::string_view font_type) -> bool
{
    auto color = app_data->config.Get_Color_Value("cursor", "color");
    m_color.r = color.r;
    m_color.g = color.g;
    m_color.b = color.b;
    m_color.a = color.a;
    m_width = app_data->config.Get_Int_Value("cursor", "width");
    m_renderer = app_data->renderer;
    m_font = app_data->fonts.at(font_type);
    m_data = data;

    switch (m_data->type) {
    case Cursor::Type::Beam:
        return Beam();
    case Cursor::Type::Box:
        return Box();
    case Cursor::Type::Underline:
        return Underline();
    default:
        return false;
    }
}


auto
Renderer::Beam() -> bool
{
    std::string behind = m_data->line.substr(0, m_data->cursor.x);

    int32_t height = TTF_GetFontHeight(m_font);
    int32_t x = 0;

    if (m_data->cursor.x > 0) {
        SDL::Get_String_Size(m_font, behind, &x, nullptr);
    }

    SDL_FRect rect = {
        static_cast<float>(m_data->line_offset.x + x),
        static_cast<float>(m_data->line_offset.y + (m_data->cursor.y * height)),
        static_cast<float>(m_width),
        static_cast<float>(height)
    };

    return SDL::Draw_Rect_Outline({ m_renderer, m_color }, rect, { 0, 0, 0, m_width });
}


auto
Renderer::Box() -> bool
{
    std::string behind = m_data->line.substr(0, m_data->cursor.x);
    char hovered_char = ' ';
    if (m_data->cursor.x < m_data->line.length()) {
        hovered_char = m_data->line.at(m_data->cursor.x);
    }

    int32_t height = 0;
    int32_t w = 0;
    int32_t x = 0;
    SDL::Get_Char_Size(m_font, hovered_char, &w, &height);

    if (m_data->cursor.x > 0) {
        SDL::Get_String_Size(m_font, behind, &x, nullptr);
    }

    SDL_FRect rect = {
        static_cast<float>(m_data->line_offset.x + x),
        static_cast<float>(m_data->line_offset.y + (m_data->cursor.y * height)),
        static_cast<float>(w),
        static_cast<float>(height)
    };

    return SDL::Draw_Rect_Outline(
        { m_renderer, m_color }, rect, { m_width, m_width, m_width, m_width }
    );
}


auto
Renderer::Underline() -> bool
{
    std::string behind = m_data->line.substr(0, m_data->cursor.x);
    char hovered_char = m_data->line.at(m_data->cursor.x);

    int32_t height = 0;
    int32_t width = 0;
    int32_t x = 0;
    SDL::Get_Char_Size(m_font, hovered_char, &width, &height);

    if (m_data->cursor.x > 0) {
        SDL::Get_String_Size(m_font, behind, &x, nullptr);
    }

    SDL_FRect rect = {
        static_cast<float>(m_data->line_offset.x + x),
        static_cast<float>(m_data->line_offset.y + (m_data->cursor.y * height)),
        static_cast<float>(width),
        static_cast<float>(height)
    };

    return SDL::Draw_Rect_Outline({ m_renderer, m_color }, rect, { 0, 0, width, 0 });
}
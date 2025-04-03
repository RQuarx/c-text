#include <cmath>

#include <SDL3_ttf/SDL_ttf.h>

#include "../inc/logging_utility.hpp"
#include "../inc/cursor.hpp"

#include "../inc/editor.hpp"

using Editor::UI;


UI::UI(
    bool *return_code,
    std::string &file_path,
    Cursor::Renderer *cursor_renderer
) : m_cursor_renderer(cursor_renderer)
{
    std::filesystem::path file = std::filesystem::path(file_path);
    m_editor_data = std::make_unique<Data>(file);
    *return_code = true;
}


auto
UI::Get_Data() -> Data*
{ return m_editor_data.get(); }


auto
UI::Init(
    UI *editor_ui,
    std::string &file_path,
    Cursor::Renderer *cursor_renderer,
    bool debug
) -> bool
{
    if (debug) Log::Debug(stdout, "Initialising editor: ");
    bool return_code = false;

    *editor_ui = UI(&return_code, file_path, cursor_renderer);

    if (!return_code) {
        Log::Failed_Msg();
        Log::Err("File path is not valid: {}", file_path);
        return return_code;
    }

    if (debug) Log::Success_Msg();
    return return_code;
}


auto
UI::Render(AppData *app_data) -> bool
{
    int32_t window_height = 0;
    int32_t window_width = 0;
    int32_t line_height = TTF_GetFontHeight(app_data->fonts.at("editor"));

    if (!SDL::Get_Window_Size_Px(app_data->window, &window_width, &window_height)) {
        return false;
    }

    m_editor_data->last_rendered_line = std::min(
        m_editor_data->scroll.y + ((window_height - m_editor_data->position.y) / line_height),
        static_cast<int64_t>(m_editor_data->file_content.size())
    );

    m_editor_data->max_editor_width = window_width - m_editor_data->position.x;

    /* Offset used to render text line by line initialised with the editor's position */
    int32_t y_offset = m_editor_data->position.y;
    for (size_t i = m_editor_data->scroll.y; i < m_editor_data->last_rendered_line; i++) {
        int32_t line_number_width = 0;
        Render_Line_Number(app_data, i, { m_editor_data->position.x, y_offset }, &line_number_width);

        Position render_pos = { m_editor_data->position.x + line_number_width, y_offset };
        Render_Text(app_data, render_pos, i);

        Position cursor_pos = {
            render_pos.x,
            m_editor_data->position.y
        };

        std::string line = m_editor_data->file_content.at(m_editor_data->cursor.y);

        Render_Cursor(app_data, cursor_pos, line);

        y_offset += line_height;
    }

    return true;
}


auto
UI::Render_Line_Number(
    AppData *app_data,
    int64_t line_index,
    Position pos,
    int32_t *line_number_width
) const -> bool
{
    int64_t line = line_index;
    bool zero_indexing = app_data->config.Get_Bool_Value("editor", "zero_indexing");
    bool relative = app_data->config.Get_Bool_Value("editor", "relative_line_number");
    bool is_current_line = (m_editor_data->cursor.y == line_index);
    bool padding = (
        app_data->config.Get_Bool_Value("editor", "current_line_padding") &&
        is_current_line
    );
    size_t file_size = m_editor_data->file_content.size();

    if (line_index < m_editor_data->cursor.y) {
        line_index++;
    }

    std::string text;
    if (relative && !is_current_line) {
        text = std::to_string(std::abs(m_editor_data->cursor.y - line) - (zero_indexing ? 1 : 0));
    } else {
        text = std::to_string(line - (zero_indexing ? 0 : -1));
    }

    int32_t total_spaces = std::log10(file_size) + 4 - text.length() - (padding ? 2 : 0);
    text.insert(0, total_spaces, ' ');

    if (padding) text += "  ";
    text += "  ";

    TTF_Font *font = app_data->fonts.at("editor");
    SDL_Color color = (is_current_line
        ? app_data->config.Get_Color_Value("editor", "foreground")
        : app_data->config.Get_Color_Value("editor", "alt_foreground")
    );
    if (!SDL::Draw_Text({ app_data->renderer, font, color, pos }, text, nullptr)) {
        return false;
    }

    return SDL::Get_String_Size(font, text, line_number_width, nullptr);
}


auto
UI::Render_Text(AppData *app_data, Position position, int64_t line_index) const -> bool
{
    SDL_Color color = app_data->config.Get_Color_Value("editor", "foreground");
    TTF_Font *font = app_data->fonts.at("editor");

    return SDL::Draw_Text_Closed(
        { app_data->renderer, font, color, position },
        m_editor_data->max_editor_width,
        m_editor_data->file_content.at(line_index),
        nullptr
    );
}


auto
UI::Render_Cursor(AppData *app_data, Position position, std::string &line) const -> bool
{
    position.y -= (m_editor_data->scroll.y * TTF_GetFontHeight(app_data->fonts.at("editor")));
    Cursor::Data cursor_data(line, position, m_editor_data->cursor, Cursor::Type::Box);

    if (m_editor_data->mode == Command) return true;

    if (m_editor_data->mode == Replace) { cursor_data.type = Cursor::Type::Underline; }
    else if (m_editor_data->mode == Insert) { cursor_data.type = Cursor::Type::Beam; }

    return m_cursor_renderer->Render(app_data, &cursor_data, "editor");
}
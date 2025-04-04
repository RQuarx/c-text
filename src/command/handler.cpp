#include "../../inc/command.hpp"

using Command::Handler;


void
Handler::Init(Cursor::Renderer *cursor_renderer)
{ this->cursor_renderer = cursor_renderer; }


void
Handler::Update_Command(std::string &str)
{
    command.insert(cursor.x, str);
    cursor.x += str.length();
}


void
Handler::Clear_Command()
{
    command.clear();
    command = ":";
    cursor.x = 1;
}


auto
Handler::Render(AppData *app_data) -> bool
{
    if (command == "::") {
        command = ":";
        cursor.x--;
    }
    SDL_Color bg = app_data->config.Get_Color_Value("command", "background");
    int32_t padding = app_data->config.Get_Int_Value("command", "padding");
    TTF_Font *font = app_data->fonts.at("command");
    std::array<int32_t, 2> window_size{ 0 };
    int32_t text_height = 0;

    if (
        !SDL::Get_Window_Size_Px(app_data->window, &window_size.at(0), &window_size.at(1))
    ) { return false; }

    if (!SDL::Get_String_Size(font, command, nullptr, &text_height)) { return false; }
    if (!SDL::Set_Draw_Color(app_data->renderer, bg)) { return false; }

    int32_t panel_height = text_height;
    SDL_FRect panel = {
        0.0F,
        static_cast<float>(window_size.at(1) - (panel_height + padding)),
        static_cast<float>(window_size.at(0)),
        static_cast<float>(panel_height + padding)
    };

    if (!SDL::Draw_Filled_Rect(app_data->renderer, bg, &panel)) { return false; }

    panel.y -= 1.0F;

    {
        int32_t border_width = app_data->config.Get_Int_Value("command", "border_width");
        SDL_Color border = app_data->config.Get_Color_Value("command", "border");
        if (
            !SDL::Draw_Rect_Outline(
                { app_data->renderer, border }, panel, { border_width, 0, 0, 0 }
            )
        ) { return false; }
    }

    panel.y += 1.0F;

    float text_y = panel.y + ((panel_height + padding - text_height) / 2.0F);

    if (command.empty()) command = ":";

    {
        SDL_Color fg = app_data->config.Get_Color_Value("command", "foreground");
        if (!
            SDL::Draw_Text(
                { app_data->renderer, font, fg, { static_cast<int64_t>(panel.x), static_cast<int64_t>(text_y) } },
                command,
                nullptr
            )
        ) { return false; }
    }

    Cursor::Data cursor_data(
        command,
        cursor,
        { static_cast<int64_t>(panel.x), static_cast<int64_t>(text_y) },
        Cursor::Type::Beam
    );
    return cursor_renderer->Render(app_data, &cursor_data, "editor");
}


auto
Handler::Get_Command() -> std::string_view
{
    std::string_view returned_str = command;
    return returned_str.substr(1);
}


auto
Handler::Handle_Backspace() -> bool
{
    if (cursor.x <= 1) return false;

    /* Handle Ctrl+Backspace */
    if ((SDL_GetModState() & SDL_KMOD_LCTRL) != 0U) {
        while (cursor.x > 1 && Utils::Is_Word_Bound(command.at(cursor.x))) {
            command.erase(cursor.x - 1, 1);
            cursor.x--;
        }

        while (cursor.x > 1 && !Utils::Is_Word_Bound(command.at(cursor.x))) {
            command.erase(cursor.x - 1, 1);
            cursor.x--;
        }
        return true;
    }

    command.erase(cursor.x - 1, 1);
    cursor.x--;
    return true;
}
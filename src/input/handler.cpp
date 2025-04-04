#include "../../inc/command.hpp"
#include "../../inc/cursor.hpp"

#include "../../inc/input.hpp"

using Input::Handler;


auto
Handler::Handle(SDL_Scancode code, Editor::UI *editor, AppData *app_data, Command::Handler *command_handler) -> bool
{
    is_lshift_pressed = (SDL_GetModState() & SDL_KMOD_LSHIFT) != 0U;
    is_lctrl_pressed = (SDL_GetModState() & SDL_KMOD_LCTRL) != 0U;
    editor_data = editor->Get_Data();
    this->code = code;
    this->app_data = app_data;

    if (Handle_Global()) return true;

    switch (editor_data->mode) {
    case Editor::Normal:
        return Handle_Normal_Mode();

    case Editor::Insert:
        return Handle_Insert_Mode();

    case Editor::Command:
        return Handle_Command_Mode(command_handler, editor_data);

    // case Visual:
    //     return Handle_Visual_Mode();

    default:
        return false;
    }
    return false;
}


auto
Handler::Handle_Global() -> bool
{
    Position *cursor = &editor_data->cursor;

    switch (code) {
    case SDL_SCANCODE_RIGHT:
        return Cursor::Logic::Move_Cursor_Right(editor_data, is_lctrl_pressed);
    case SDL_SCANCODE_LEFT:
        return Cursor::Logic::Move_Cursor_Left(editor_data, is_lctrl_pressed);
    case SDL_SCANCODE_DOWN:
        return Cursor::Logic::Move_Cursor_Down(editor_data, is_lctrl_pressed);
    case SDL_SCANCODE_UP:
        return Cursor::Logic::Move_Cursor_Up(editor_data, is_lctrl_pressed);

    case SDL_SCANCODE_END: {
        size_t line_len = editor_data->file_content.at(cursor->y).length();
        if (line_len == 0) return false;

        if (editor_data->mode != Editor::Insert) { cursor->x = line_len - 1; }
        else { cursor->x = line_len; }
        return true;
    }
    case SDL_SCANCODE_HOME:
        if (cursor->x == 0) return false;
        cursor->x = 0;
        return true;

    default:
        return false;
    }
}


auto
Handler::Handle_Insert_Mode() -> bool
{
    switch (code) {
    case SDL_SCANCODE_ESCAPE:
        SDL_StopTextInput(app_data->window);
        editor_data->mode = Editor::Normal;
        if (editor_data->cursor.x > 0) editor_data->cursor.x--;
        return true;

    case SDL_SCANCODE_BACKSPACE:
        return Input::Logic::Handle_Backspace(editor_data, is_lctrl_pressed);

    case SDL_SCANCODE_RETURN:
        return Input::Logic::Handle_Return(editor_data);

    case SDL_SCANCODE_TAB: {
        int32_t tab_size = app_data->config.Get_Int_Value("file", "tab_size");
        editor_data->file_content.at(editor_data->cursor.y).insert(editor_data->cursor.x, std::string(tab_size, ' '));
        editor_data->cursor.x += tab_size;
        return true;
    }

    default:
        return false;
    }
}


auto
Handler::Handle_Normal_Mode() -> bool
{
    switch (code) {
    case SDL_SCANCODE_I:
        SDL_StartTextInput(app_data->window);
        editor_data->mode = Editor::Insert;
        return true;

    case SDL_SCANCODE_A:
        SDL_StartTextInput(app_data->window);
        {
            auto *cursor = &editor_data->cursor;
            if (cursor->x < editor_data->file_content.at(cursor->y).length()) cursor->x++;
        }
        editor_data->mode = Editor::Insert;
        return true;

    case SDL_SCANCODE_SEMICOLON:
        if (is_lshift_pressed) {
            SDL_StartTextInput(app_data->window);
            editor_data->mode = Editor::Command;
            return true;
        }
        return false;

    case SDL_SCANCODE_L:
        return Cursor::Logic::Move_Cursor_Right(editor_data, is_lctrl_pressed);
    case SDL_SCANCODE_H:
        return Cursor::Logic::Move_Cursor_Left(editor_data, is_lctrl_pressed);
    case SDL_SCANCODE_J:
        return Cursor::Logic::Move_Cursor_Down(editor_data, is_lctrl_pressed);
    case SDL_SCANCODE_K:
        return Cursor::Logic::Move_Cursor_Up(editor_data, is_lctrl_pressed);

    default:
        return false;
    }
}


auto
Handler::Handle_Command_Mode(Command::Handler *command_handler, Editor::Data *editor_data) -> bool
{
    switch (code) {
    case SDL_SCANCODE_RETURN: {
        std::string cmd(command_handler->Get_Command());
        if (!Command::Logic::Handle(cmd, editor_data, app_data)) return false;
        command_handler->Clear_Command();

        editor_data->mode = Editor::Normal;
        return true;
    }

    case SDL_SCANCODE_ESCAPE:
        command_handler->Clear_Command();
        editor_data->mode = Editor::Normal;
        return true;

    case SDL_SCANCODE_BACKSPACE:
        return command_handler->Handle_Backspace();

    default:
        return false;
    }
}
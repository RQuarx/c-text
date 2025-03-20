#pragma once

#include <SDL3/SDL.h>

#include "editor.hpp"


class
InputHandler
{
public:
    InputHandler() = default;

    /// Handles input from an SDL_Scancode
    /// @param code SDL_Scancode type
    /// @param editor editor class used
    /// @returns true on "should render", or false on "do nothing"
    auto Handle(SDL_Scancode code, Editor *editor, AppData *app_data, CommandHandler *command_handler) -> bool;

private:
    bool is_lshift_pressed;
    bool is_lctrl_pressed;
    SDL_Scancode code;
    EditorData *editor_data;
    AppData *app_data;

    /// Handles all default inputs that doesnt need to be on a certain editor mode
    /// @returns true on "should render", or false on "do nothing"
    auto Handle_Global() -> bool;

    auto Handle_Normal_Mode() -> bool;
    auto Handle_Insert_Mode() -> bool;
    auto Handle_Visual_Mode() -> bool;
    auto Handle_Command_Mode(CommandHandler *command_handler, EditorData *editor_data) -> bool;
};
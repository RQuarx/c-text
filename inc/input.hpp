#pragma once

#include "command.hpp"
#include "editor.hpp"


namespace Input {
    class
    Handler
    {
    public:
        Handler() = default;

        /// Handles input from an SDL_Scancode
        /// @param code SDL_Scancode type
        /// @param editor editor class used
        /// @returns true on "should render", or false on "do nothing"
        auto Handle(SDL_Scancode code, Editor::UI *editor, AppData *app_data, Command::Handler *command_handler) -> bool;

    private:
        bool is_lshift_pressed;
        bool is_lctrl_pressed;
        SDL_Scancode code;
        Editor::Data *editor_data;
        AppData *app_data;

        /// Handles all default inputs that doesnt need to be on a certain editor mode
        /// @returns true on "should render", or false on "do nothing"
        auto Handle_Global() -> bool;

        auto Handle_Normal_Mode() -> bool;
        auto Handle_Insert_Mode() -> bool;
        auto Handle_Visual_Mode() -> bool;
        auto Handle_Command_Mode(Command::Handler *command_handler, Editor::Data *editor_data) -> bool;
    };

    class
    Logic
    {
    public:
        static auto Handle_Backspace(Editor::Data *editor_data, bool is_lctrl_pressed) -> bool;
        static auto Handle_Return(Editor::Data *editor_data) -> bool;

    private:
        static void Handle_Ctrl_Backspace(Editor::Data *editor_data);
    };
} /* namespace Input */
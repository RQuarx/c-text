#pragma once

#include "sdl_helper.hpp"
#include "cursor.hpp"


namespace Command {
    class
    Handler
    {
    public:
        explicit Handler(Cursor::Renderer *cursor_renderer) : cursor_renderer(cursor_renderer) {}
        Handler() = default;

        void Init(Cursor::Renderer *cursor_renderer);

        void Update_Command(std::string &str);

        void Clear_Command();

        auto Get_Command() -> std::string_view;

        [[nodiscard]]
        auto Render(AppData *app_data) -> bool;

        [[nodiscard]]
        auto Handle_Backspace() -> bool;

    private:
        Cursor::Renderer *cursor_renderer{};
        std::string command;
        Position cursor{1, 0};
    };

    namespace Logic {
        auto Handle(std::string &cmd, Editor::Data *editor_data, AppData *app_data) -> bool;
    }
} /* namespace Command */
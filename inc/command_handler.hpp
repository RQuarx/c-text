#pragma once

#include "../inc/cursor_renderer.hpp"
#include "sdl_helper.hpp"


static const uint8_t MAX_COMMAND_SIZE = 20;


class
CommandHandler
{
public:
    explicit CommandHandler(CursorRenderer *cursor_renderer) : cursor_renderer(cursor_renderer) {}
    CommandHandler() = default;

    void Init(CursorRenderer *cursor_renderer);

    void Update_Command(std::string &str);

    void Clear_Command();

    auto Get_Command() -> std::string_view;

    [[nodiscard]]
    auto Render(AppData *app_data) -> bool;

    [[nodiscard]]
    auto Handle_Backspace() -> bool;

private:
    CursorRenderer *cursor_renderer{};
    std::string command;
    Position cursor{1, 0};
};
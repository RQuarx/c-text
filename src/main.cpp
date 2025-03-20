#include <print>

#include "../inc/argument_parser.hpp"
#include "../inc/command_handler.hpp"
#include "../inc/logging_utility.hpp"
#include "../inc/config_parser.hpp"
#include "../inc/input_handler.hpp"
#include "../inc/file_handler.hpp"
#include "../inc/sdl_helper.hpp"
#include "../inc/utilities.hpp"
#include "../inc/editor.hpp"

static const float ONE_SECOND_MS = 1000.0F;
static const char *const APP_NAME = "c+text";
static const char *const APP_VERSION = "0.0.1";
static const char *const APP_DESCRIPTION = "Simple Text Editor";


enum AppResult : uint8_t {
    Exit_Failure,
    Exit_Success,
    Continue_Render,
    Continue_Skip
};


namespace {
    auto
    App_Render(AppData *app_data, Editor *editor, CommandHandler *command) -> bool
    {
        if (
            !SDL::Set_Draw_Color_Blend(
                app_data->renderer,
                app_data->config.Get_Color_Value("ui", "background")
            )
        ) return false;
        SDL_RenderClear(app_data->renderer);

        if (!editor->Render(app_data)) return false;

        if (editor->Get_Data()->mode == EditorMode::Command) {
            if (!command->Render(app_data)) return false;
        }

        SDL_RenderPresent(app_data->renderer);
        return true;
    }


    auto
    Handle_Mouse_Wheel(SDL_MouseWheelEvent wheel_event, Editor *editor) -> bool
    {
        auto direction = wheel_event.y;
        auto *scroll = &editor->Get_Data()->scroll;

        if (direction > 0 && scroll->y > 0) {
            scroll->y--;
            return true;
        }

        if (direction < 0 && scroll->y < static_cast<int64_t>(editor->Get_Data()->file_content.size())) {
            scroll->y++;
            return true;
        }

        return false;
    }


    auto
    App_Event(SDL_Event *event, AppData *app_data, InputHandler *input_handler, Editor *editor, CommandHandler *command) -> AppResult
    {
        while (SDL_PollEvent(event)) {
            switch (event->type) {
            case SDL_EVENT_QUIT:
                return Exit_Success;

            case SDL_EVENT_MOUSE_WHEEL:
                if (Handle_Mouse_Wheel(event->wheel, editor)) {
                    return Continue_Render;
                } else { return Continue_Skip; }

            case SDL_EVENT_TEXT_INPUT: {
                auto *data = editor->Get_Data();
                std::string text = event->text.text;

                if (data->mode == Command) {
                    command->Update_Command(text);
                    return Continue_Render;
                }

                if (data->mode == Insert) {
                    data->file_content.at(data->cursor.y).insert(data->cursor.x, text);
                    data->cursor.x += text.length();
                    data->cursor_max_x = data->cursor.x;
                }
                return Continue_Render;
            }

            case SDL_EVENT_KEY_DOWN:
                return (
                    input_handler->Handle(event->key.scancode, editor, app_data, command) ? Continue_Render : Continue_Skip
                );

            case SDL_EVENT_WINDOW_RESIZED:
                return Continue_Render;
            default:
                return Continue_Skip;
            }
        }
        return Continue_Skip;
    }


    void
    Parse_Arg(ArgParser *arg_parser, bool *debug)
    {
        if (arg_parser->Find_Arg({ "-h", "--help" })) {
            ArgParser::Print_Help_Message(stdout);
        }

        if (arg_parser->Find_Arg({ "-v", "--version" })) {
            std::println("{}-{}", APP_NAME, APP_VERSION);
            exit(EXIT_SUCCESS);
        }

        *debug = arg_parser->Find_Arg({ "-d", "--debug" });
    }


    auto
    App_Init(
        AppData *app_data,
        Editor *editor,
        CursorRenderer *cursor_renderer,
        CommandHandler *command,
        ConfigParser *config,
        ArgParser *arg_parser
    ) -> bool
    {
        Log::Info("Initialisation started...\n");

        /* Finding / getting the file to edit */
        std::string file_path;
        arg_parser->Get_File_Path(config, file_path);

        if (!Editor::Init_Editor(editor, file_path, cursor_renderer, app_data->debug)) return false;

        std::string window_title = (
            config->Get_Value("window", "static_title") == "yes" ?
            APP_NAME :
            std::filesystem::relative(file_path)
        );

        app_data->config = *config;

        command->Init(cursor_renderer);

        AppInfo app_info = { APP_NAME, APP_VERSION, APP_DESCRIPTION };
        if (!SDL::Init(app_data, &app_info, window_title)) {
            SDL::Kill(app_data);
            return false;
        }

        auto buff = File::Parse_File(file_path, config->Get_Int_Value("file", "tab_size"));

        if (buff.empty()) {
            editor->Get_Data()->file_content.assign(1, "");
        } else {
            editor->Get_Data()->file_content = buff;
        }


        Log::Info("Initialitation completed, starting rendering process\n");
        return true;
    }
} /* Anonymous namespace */


auto
main(int32_t argc, char **argv) -> int32_t
{
    bool debug = false;

    /* Parsing command line arguments */
    ArgParser arg_parser(argc, argv);
    Parse_Arg(&arg_parser, &debug);

    CursorRenderer cursor_renderer{};
    InputHandler input_handler{};
    CommandHandler command;
    AppData app_data;
    Editor editor;

    app_data.debug = debug;

    /* Parsing config file */
    ConfigParser config;
    if (!ConfigParser::Init_Config(&config, &arg_parser, debug)) return EXIT_FAILURE;

    /* Initialises everything */
    if (!App_Init(&app_data, &editor, &cursor_renderer, &command, &config, &arg_parser)) {
        return EXIT_FAILURE;
    }

    /* Rendering */
    AppResult result = Continue_Render;
    bool first_start = true;
    SDL_Event event;
    while (true) {
        SDL_Delay(ONE_SECOND_MS / app_data.display_mode->refresh_rate);
        result = App_Event(&event, &app_data, &input_handler, &editor, &command);

        if (result == Exit_Failure || result == Exit_Success) break;
        if (first_start || result == Continue_Render) {
            if (first_start) first_start = false;
            if (!App_Render(&app_data, &editor, &command)) break;
        }

    }

    SDL::Kill(&app_data);
    return EXIT_SUCCESS;
}
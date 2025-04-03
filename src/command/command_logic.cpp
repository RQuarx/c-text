#include "../../inc/logging_utility.hpp"
#include "../../inc/file_handler.hpp"
#include "../../inc/editor.hpp"

#include "../../inc/command.hpp"


namespace Command::Logic {
    auto
    Handle(std::string &cmd, Editor::Data *editor_data, AppData *app_data) -> bool
    {
        if (cmd == "w" || cmd == "wq") {
            if (!File::Write_File(editor_data->file_path, editor_data->file_content)) {
                Log::Err("Failed to write to file: {}", editor_data->file_path.string());
                return false;
            }
            if (cmd == "w") return true;
        }

        if (cmd == "q" || cmd == "wq") {
            SDL::Kill(app_data);
            exit(EXIT_SUCCESS);
        }

        return true;
    }
} /* namespace Command::Logic */
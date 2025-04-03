#include <fstream>

#include "../inc/utilities.hpp"
#include "../inc/logging_utility.hpp"

#include "../inc/file_handler.hpp"


namespace File {
    auto
    Parse_File(std::string &file_path, int32_t tab_size, bool first_init) -> std::vector<std::string>
    {
        std::vector<std::string> file_content;

        if (!Utils::Is_Valid_File(file_path)) {
            return file_content; /* Returns an empty vector */
        }

        std::ifstream file(file_path);

        if (!file.is_open()) {
            if (first_init) Log::Failed_Msg();
            Log::Err("Failed to open file: {}", file_path);
            return file_content; /* Returns an empty vector */
        }

        std::string line;
        while (std::getline(file, line)) {
            Utils::Trim_String(line, Right);

            /* Changes all \t characters */
            size_t position = 0;
            while ((position = line.find('\t', position)) != std::string::npos) {
                line.replace(position, 1, std::string(tab_size, ' '));
            }

            file_content.emplace_back(std::move(line));
        }

        return file_content;
    }


    auto
    Parse_File_Async(std::string &file_path, int32_t tab_size) -> std::future<std::vector<std::string>>
    {
        return std::async(std::launch::async, [file_path, tab_size]() -> std::vector<std::string> {
            std::vector<std::string> file_content;

            if (!Utils::Is_Valid_File(file_path)) {
                return { " " }; /* Returns an empty vector */
            }

            std::ifstream file(file_path);

            if (!file.is_open()) {
                Log::Err("Failed to open file: {}", file_path);
                return file_content; /* Returns an empty vector */
            }

            std::string line;
            while (std::getline(file, line)) {
                if (!Utils::Trim_String(line, Right)) {
                    line = " ";
                }

                /* Changes all \t characters */
                size_t position = 0;
                while ((position = line.find('\t', position)) != std::string::npos) {
                    line.replace(position, 1, std::string(tab_size, ' '));
                }

                file_content.emplace_back(std::move(line));
            }

            file.close();
            return file_content;
        });
    }


    auto
    Write_File(
        std::filesystem::path &file_path,
        const std::vector<std::string> &file_content
    ) -> bool
    {
        if (!Utils::Is_Valid_File(file_path.string()) && file_path.string().contains('/')) {
            std::filesystem::create_directory(file_path.relative_path());
        }

        std::ofstream file(file_path);

        if (!file.is_open()) {
            Log::Err("Failed to open file: {}", file_path.string());
            return false;
        }

        for (size_t i = 0; i < file_content.size(); i++) {
            std::string inserted_string = file_content[i];
            if (i != file_content.size() - 1) inserted_string += "\n";
            file << inserted_string;
        }

        return true;
    }
}  /* namespace File */
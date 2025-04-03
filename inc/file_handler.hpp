#pragma once

#include <filesystem>
#include <future>
#include <string>
#include <vector>


namespace File {

    /// Parses a file and put the text into a std::vector<std::string> type
    /// @param file_path the path to the specified file, must be of type 'regular_file'
    /// @param tab_size the amount of spaces that will be used to replace the \t character
    /// @return will return an empty vector on failure
    auto Parse_File(std::string &file_path, int32_t tab_size, bool first_init = false) -> std::vector<std::string>;


    /// Parses a file and put the text into a std::vector<std::string> type
    /// @param file_path the path to the specified file, must be of type 'regular_file'
    /// @param tab_size the amount of spaces that will be used to replace the \t character
    /// @returns A future that will hold a vector of strings or an emptyt one.
    auto Parse_File_Async(std::string &file_path, int32_t tab_size) -> std::future<std::vector<std::string>>;

    /// Writes / save the file content to the file_path
    /// @param file_path the path to the file that will be written to
    /// @param file_content the new content of the file
    /// @returns true on success or false on failure.
    auto Write_File(
        std::filesystem::path &file_path,
        const std::vector<std::string> &file_content
    ) -> bool;
} /* namespace File */
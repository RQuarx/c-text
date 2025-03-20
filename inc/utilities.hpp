#pragma once

#include <filesystem>
#include <cstdint>
#include <string>


namespace Color {
    static const char *const Reset = "\e[0;0;0m";

    static const char *const Magenta = "\e[0;35m";
    static const char *const Yellow = "\e[0;33m";
    static const char *const Black = "\e[0;30m";
    static const char *const Green = "\e[0;32m";
    static const char *const White = "\e[0;37m";
    static const char *const Blue = "\e[0;34m";
    static const char *const Cyan = "\e[0;36m";
    static const char *const Red = "\e[0;31m";

    static const char *const Bold_Magenta = "\e[1;35m";
    static const char *const Bold_Yellow = "\e[1;33m";
    static const char *const Bold_Black = "\e[1;30m";
    static const char *const Bold_Green = "\e[1;32m";
    static const char *const Bold_White = "\e[1;37m";
    static const char *const Bold_Blue = "\e[1;34m";
    static const char *const Bold_Cyan = "\e[1;36m";
    static const char *const Bold_Red = "\e[1;31m";
} /* namespace Color */

enum Direction : uint8_t {
    All,
    Left,
    Right,
};


struct Position {
    int64_t x;
    int64_t y;

    Position(int64_t _x, int64_t _y) : x(_x), y(_y) {}
    Position() : x(0), y(0) {}

    [[nodiscard]]
    auto Is_Zero() const -> bool
    { return x == 0 && y == 0; }
};

namespace Utils {

    /// Returns the current time in minute:second:milisecond
    auto Get_Current_Time() -> std::string;

    /// Trims input string
    /// @param trim_direction options = All, Left, Right
    /// @returns true on success or false on failure.
    auto Trim_String(std::string &str, Direction trim_direction) -> bool;

    /// Match the input font name with a corresponding font file
    /// @return will return a const char * of a path or nullptr
    auto Match_Font(const std::string &font_name) -> const char*;

    /// Checks if a path / file is a valid text file
    auto Is_Valid_File(const std::string &file_path) -> bool;

    /// Checks if c is a word bound
    auto Is_Word_Bound(char c) -> bool;

    /// Checks if checked_string is only composed of whitespaces
    auto Is_All_Space(std::string_view checked_string) -> bool;

    auto Path_To_String(const std::filesystem::path &path) -> std::string;
    auto String_To_Path(const std::string &utf8_string) -> std::filesystem::path;
} /* namespace Utils */
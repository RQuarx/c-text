#pragma once

#include <unordered_map>
#include <filesystem>

#include <SDL3/SDL.h>

#include "argument_parser.hpp"

using config_section = std::unordered_map<std::string, std::string>;
using config_data = std::unordered_map<std::string, config_section>;

#if __unix__
    static const std::filesystem::path DEFAULT_CONFIG_PATH = std::filesystem::path(getenv("HOME")) / ".config/c+text/config.ini";
#elif _WIN32
    static const std::filesystem::path DEFAULT_CONFIG_PATH = std::filesystem::path(getenv("USERPROFILE")) / "c+text/config.ini";
#else
#   error "Unsupported platform!"
#endif

static const int32_t HEX_EXPAND_FACTOR = 17;
static const int32_t MAX_HEX_VALUE = 255;
static const int32_t RRGGBBAA = 8;
static const int32_t RRGGBB = 6;
static const int32_t RGBA = 4;
static const int32_t HEX = 16;
static const int32_t RGB = 3;


class
ConfigParser
{
public:
    /// Initialises ConfigParser
    /// @returns true on success or false on failure.
    static auto Init_Config(ConfigParser *config, ArgParser *arg_parser, bool debug) -> bool;

    /// Parses the config file
    /// @returns true on success or false on failure.
    auto Parse_Config(std::filesystem::path &config_path) -> bool;

    /// Returns the value of a given section and key from the config file as a string
    /// @return will return an empty string_view when key/section are not found
    auto Get_Value(std::string_view section, std::string_view key) -> std::string;

    /// Returns the value of a given section and key from the config file as an int64_t
    /// @return will return INT64_MIN when key/section are not found
    auto Get_Int_Value(std::string_view section, std::string_view key) -> int64_t;

    /// Returns the value of a given section an key from the config file as a bool
    /// @return when the value is yes return true, else false
    auto Get_Bool_Value(std::string_view section, std::string_view key) -> bool;

    /// Returns the value of a given section an key from the config file as a SDL_Color type
    /// @return will return the colour black if the color value is invalid
    auto Get_Color_Value(std::string_view section, std::string_view key) -> SDL_Color;

private:
    config_data data;
};
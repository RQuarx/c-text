#include <fstream>
#include <sstream>
#include <string>

#include "../inc/logging_utility.hpp"
#include "../inc/utilities.hpp"

#include "../inc/config_parser.hpp"


auto
ConfigParser::Init_Config(ConfigParser *config, ArgParser *arg_parser, bool debug) -> bool
{
    if (debug) Log::Debug(stdout, "Loading config: ");

    std::string config_file;
    if (
        !arg_parser->Option_Arg(
            config_file,
            { "-c", "--config" }
        )
    ) { config_file = DEFAULT_CONFIG_PATH; }

    std::filesystem::path config_path = std::filesystem::path(config_file);

    if (
        !std::filesystem::exists(config_path) &&
        !std::filesystem::is_regular_file(config_path)
    ) {
        if (debug) Log::Failed_Msg();
        Log::Err("Invalid config file: {}", config_file.c_str());
        return false;
    }

    config->Parse_Config(config_path);
    if (debug) Log::Success_Msg();
    return true;
}


auto
ConfigParser::Parse_Config(std::filesystem::path &config_path) -> bool
{
    if (
        !std::filesystem::exists(config_path) &&
        !std::filesystem::is_regular_file(config_path)
    ) {
        Log::Err("Config path doesnt not exist or is not a valid file!");
        return false;
    }

    std::ifstream config_file(config_path);

    if (!config_file.is_open()) {
        Log::Err("Failed to open config file!");
        return false;
    }

    std::string line;
    std::string current_section;

    while (std::getline(config_file, line)) {
        if (!Utils::Trim_String(line, All)) continue;

        if (line.at(0) == '[' && line.back() == ']') {
            current_section = line.substr(1, line.length() - 2);
            continue;
        }

        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            Utils::Trim_String(key, All);
            Utils::Trim_String(value, All);

            if (!current_section.empty()) {
                data[current_section][key] = value;
            }
        }
    }

    return true;
}


auto
ConfigParser::Get_Value(std::string_view section, std::string_view key) -> std::string
{
    auto section_it = data.find(std::string(section));
    if (section_it != data.end()) {
        auto key_it = section_it->second.find(std::string(key));
        if (key_it != section_it->second.end()) {
            return key_it->second;
        }
    }

    Log::Err("Key / Section does not exist: {}, {}", section, key);
    return "";
}


auto
ConfigParser::Get_Int_Value(std::string_view section, std::string_view key) -> int64_t
{
    std::string value = Get_Value(section, key);
    if (value.empty()) {
        Log::Err("Invalid section / key");
        return INT64_MIN;
    }

    try {
        return std::stoi(value);
    } catch (const std::invalid_argument&) {
        Log::Err("Input string is not a valid integer");
        return INT64_MIN;
    } catch (const std::out_of_range&) {
        Log::Err("Value is out of range from int64");
        return INT64_MIN;
    }
}


auto
ConfigParser::Get_Bool_Value(std::string_view section, std::string_view key) -> bool
{
    return Get_Value(section, key) == "yes";
}


auto
ConfigParser::Get_Color_Value(std::string_view section, std::string_view key) -> SDL_Color
{
    SDL_Color color = { 0, 0, 0, MAX_HEX_VALUE };
    std::string hex = Get_Value(section, key);
    if (hex.empty()) {
        Log::Err("Invalid section / key");
        return color;
    }
    if (hex[0] != '#') {
        Log::Err("Value is not a hex value");
        return color;
    }

    hex = hex.substr(1);

    if (hex.length() > RGBA) {
        std::stringstream ss;
        ss << std::hex << hex;

        if (hex.length() == RRGGBB) {
            uint32_t rgb = 0;
            ss >> rgb;

            color.r = (rgb >> HEX) & MAX_HEX_VALUE;
            color.g = (rgb >> HEX / 2) & MAX_HEX_VALUE;
            color.b = rgb & MAX_HEX_VALUE;
        } else if (hex.length() == RRGGBBAA) {
            uint32_t rgba = 0;
            ss >> rgba;

            color.r = (rgba >> HEX / 2 * 3) & MAX_HEX_VALUE;
            color.g = (rgba >> HEX) & MAX_HEX_VALUE;
            color.b = (rgba >> HEX / 2) & MAX_HEX_VALUE;
            color.a = rgba & MAX_HEX_VALUE;
        }
        return color;
    }

    if (hex.length() == RGB) {
        color.r = std::stoi(std::string(1, hex.at(0)), nullptr, HEX) * HEX_EXPAND_FACTOR;
        color.g = std::stoi(std::string(1, hex.at(1)), nullptr, HEX) * HEX_EXPAND_FACTOR;
        color.b = std::stoi(std::string(1, hex.at(2)), nullptr, HEX) * HEX_EXPAND_FACTOR;
    } else if (hex.length() == RGBA) {
        color.r = std::stoi(std::string(1, hex.at(0)), nullptr, HEX) * HEX_EXPAND_FACTOR;
        color.g = std::stoi(std::string(1, hex.at(1)), nullptr, HEX) * HEX_EXPAND_FACTOR;
        color.b = std::stoi(std::string(1, hex.at(2)), nullptr, HEX) * HEX_EXPAND_FACTOR;
        color.a = std::stoi(std::string(1, hex.at(3)), nullptr, HEX) * HEX_EXPAND_FACTOR;
    }
    return color;
}
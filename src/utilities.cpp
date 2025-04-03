#include <filesystem>
#include <algorithm>
#include <fstream>
#include <atomic>
#include <string>
#include <chrono>
#include <ctime>

#include <fontconfig/fontconfig.h>

#include "../inc/logging_utility.hpp"
#include "../inc/utilities.hpp"


static std::atomic<bool> is_check_running = true;

namespace Utils {
    auto
    Get_Current_Time() -> std::string
    {
        const int32_t MICROSECOND_UPPER_BOUND = 1000000;
        const int32_t MICROSECOND_TO_MILISECOND = 1000;

        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);

        std::tm tm_info{};
        localtime_r(&now_c, &tm_info);

        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
            now.time_since_epoch()
        ) % MICROSECOND_UPPER_BOUND;

        return std::format(
            "{:02}:{:02}:{:03}",
            tm_info.tm_min, tm_info.tm_min, microseconds.count() / MICROSECOND_TO_MILISECOND
        );
    }


    auto
    Trim_String(std::string &str, Direction trim_direction) -> bool
    {
        if (
            trim_direction == Direction::All ||
            trim_direction == Direction::Left
        ) {
            str.erase(
                str.begin(),
                std::ranges::find_if(
                    str,
                    [](unsigned char ch)
                    { return !std::isspace(ch); }
                )
            );
        }

        if (
            trim_direction == Direction::All ||
            trim_direction == Direction::Right
        ) {
            str.erase(
                std::ranges::find_if(
                    str.rbegin(),
                    str.rend(),
                    [](unsigned char ch)
                    { return !std::isspace(ch); }
                ).base(), str.end()
            );
        }
        return !str.empty();
    }


    auto
    Match_Font(const std::string &font_name) -> const char*
    {
        if (Is_Valid_File(font_name)) return font_name.c_str();
        FcConfig *config = FcInitLoadConfigAndFonts();

        FcPattern *pattern = FcNameParse(reinterpret_cast<const FcChar8*>(font_name.c_str()));
        if (pattern == nullptr) {
            Log::Err("Unable to create FcPattern");
            return nullptr;
        }

        FcConfigSubstitute(config, pattern, FcMatchPattern);
        FcDefaultSubstitute(pattern);

        FcResult result = FcResult::FcResultNoId;
        FcPattern *font = FcFontMatch(config, pattern, &result);

        if (font != nullptr) {
            FcChar8 *file = nullptr;

            if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
                return reinterpret_cast<char*>(file);
            }
            FcPatternDestroy(font);
        }
        FcPatternDestroy(pattern);
        return nullptr;
    }


    auto
    Is_Valid_File(const std::string &file_path) -> bool
    {
        return (
            std::filesystem::exists(file_path) &&
            std::filesystem::is_regular_file(file_path)
        );
    }


    auto
    Is_Word_Bound(char c) -> bool
    {
        return ((std::isspace(c) != 0) || (std::ispunct(c) != 0));
    }


    auto
    Is_All_Space(std::string_view checked_string) -> bool
    {
        return std::ranges::all_of(
            checked_string,
            [](char c) -> bool { return std::isspace(c); }
        );
    }


    auto
    Path_To_String(const std::filesystem::path &path) -> std::string
    {
        const std::u8string utf8_string = path.u8string();
        return { utf8_string.cbegin(), utf8_string.cend() };
    }


    auto
    String_To_Path(const std::string &utf8_string) -> std::filesystem::path
    {
        const std::u8string u8s(utf8_string.cbegin(), utf8_string.cend());
        return { u8s };
    }


    auto
    Is_Instance_Alone(const std::string &lock_file_path) -> bool
    {
        struct stat buffer{};
        if (stat(lock_file_path.c_str(), &buffer) == 0) return false;

        std::ofstream lock_file(lock_file_path);
        return !!lock_file;
    }


    // void
    // Listen_Tmp_File_Path(const std::string &tmp_path)
    // {
    //     while (is_check_running) {
    //         if (std::filesystem::exists(tmp_path)) {

    //         }
    //     }
    // }
} /* namespace Utils */
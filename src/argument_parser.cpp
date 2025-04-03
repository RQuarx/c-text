#include <algorithm>
#include <fstream>
#include <print>

#include "../inc/logging_utility.hpp"
#include "../inc/config_parser.hpp"
#include "../inc/utilities.hpp"

#include "../inc/argument_parser.hpp"


ArgParser::ArgParser(int32_t argc, char **argv)
{
    auto args = std::span(argv, argc);
    m_bin_path = args.front();

    uint8_t previous_type = 0;
    for (int32_t i = 0; i < argc; i++) {
        std::string_view arg = args[i];

        if (arg.starts_with("--")) {
            m_arg_list.at(1).emplace_back(false, arg.substr(2));
            previous_type = 1;
            continue;
        }

        if (arg.starts_with('-')) {
            m_arg_list.at(0).emplace_back(false, arg.substr(1));
            previous_type = 0;
            continue;
        }

        /* ? If an arg reaches this,
           ? then it'll mean that it doesnt have a '-' or "--" prefix
        */
        m_arg_list.at(previous_type).emplace_back(true, arg);
    }
}


auto
ArgParser::Find_Arg(sv_pair arg) -> bool
{
    for (uint8_t i = 0; i < 2; i++) {
        if (
            std::ranges::any_of(m_arg_list.at(i), [&arg, &i](const auto &a){
                return !a.first && a.second == (!i ? arg.first : arg.second).substr(i + 1);
            })
        ) return true;
    }

    return false;
}


auto
ArgParser::Option_Arg(std::string &option, sv_pair arg) -> bool
{
    return (
        Find_Option_Short(option, arg.first)
        || Find_Option_Long(option, arg.second)
    );
}


auto
ArgParser::Find_Option_Short(std::string &option, std::string_view short_arg) -> bool
{
    for (size_t i = 0; i < m_arg_list.at(0).size(); i++) {
        const auto &a = m_arg_list.at(0).at(i);
        if (a.first) continue;

        if (a.second.contains('=')) {
            size_t eq_pos = a.second.find_first_of('=');

            std::pair<std::string_view, std::string_view> split_arg = {
                a.second.substr(0, eq_pos), a.second.substr(eq_pos + 1)
            };

            /* ? Check for -lo=a and -o=a */
            if (split_arg.first.back() == short_arg.at(1)) {
                option = split_arg.second;
                return true;
            }
        }

        /* ? Checks for -oa */
        if (a.second.length() > 1) {
            if (a.second.at(0) == short_arg.at(1)) {
                option = a.second.substr(1);
                return true;
            }
        }

        /* ? Checks for -lo a and -o a */
        for (const auto &c : a.second) {
            if (c == short_arg.at(1) && i + 1 < m_arg_list.at(0).size()) {
                const auto &next_arg = m_arg_list.at(0).at(i + 1);
                if (next_arg.first) {
                    option = next_arg.second;
                    return true;
                }
            }
        }
    }

    option.clear();
    return false;
}


auto
ArgParser::Find_Option_Long(std::string &option, std::string_view long_arg) -> bool
{
    for (size_t i = 0; i < m_arg_list.at(1).size(); i++) {
        const auto &a = m_arg_list.at(1).at(i);
        if (a.first) continue;

        if (a.second.contains('=')) {
            size_t eq_pos = a.second.find_first_of('=');

            std::pair<std::string_view, std::string_view> split_arg = {
                a.second.substr(0, eq_pos), a.second.substr(eq_pos + 1)
            };

            /* ? Check for --option=a */
            if (split_arg.first == long_arg.substr(2)) {
                option = split_arg.second;
                return true;
            }
        }

        if (a.second == long_arg.substr(2) && i + 1 < m_arg_list.at(1).size()) {
            const auto &next_arg = m_arg_list.at(1).at(i + 1);

            if (next_arg.first) {
                option = next_arg.second;
                return true;
            }
        }
    }

    option.clear();
    return false;
}


auto
ArgParser::Get_File_Path(ConfigParser *config, std::string &file_path) -> bool
{
    if (!Option_Arg(file_path, { "-f", "--file" })) {;
        std::string back;

        for (uint8_t i = 0; i < 2; i++) {
            if (!m_arg_list.at(i).empty() && m_arg_list.at(i).back().first) {
                back = m_arg_list.at(i).back().second;
                break;
            }
        }

        if (back.empty()) {
            file_path = config->Get_Value("file", "new_file_name");
            return true;
        }

        file_path = back;
    }

    if (!Utils::Is_Valid_File(file_path)) {
        std::filesystem::path path = file_path;

        if (file_path.contains('/') || file_path.contains('\\')) {
            std::filesystem::create_directory(path.root_directory());
        }
        std::ofstream out_file(path);
        out_file << "";
    }

    std::ifstream in_file(file_path);
    return in_file.peek() == std::ifstream::traits_type::eof();
}


auto
ArgParser::Back() -> std::string
{
    if (m_arg_list.at(0).back().first) {
        Log::Info("a: {}\n", m_arg_list.at(0).back().second);
        std::fflush(stdout);
        return std::string(m_arg_list.at(0).back().second);
    }
    if (m_arg_list.at(1).back().first) {
        Log::Info("a: {}\n", "a");
        std::fflush(stdout);
        Log::Info("a: {}\n", m_arg_list.at(1).back().second);
        std::fflush(stdout);
        return std::string(m_arg_list.at(1).back().second);
    }
    return "";
}


void
ArgParser::Print_Help_Message(FILE *stream)
{
    std::println(stream, "{}c+text{}, A Simple Text Editor", Color::Bold_White, Color::Reset);
    std::println(stream, "┌──");
    std::println(stream, "├─{}Usage{}: c+text [options] [file path]", Color::Bold_White, Color::Reset);
    std::println(stream, "│");
    std::println(stream, "├─{}Options{}:", Color::Bold_White, Color::Reset);
    std::println(stream, "│      {}-h,--help{}                show this message", Color::Bold_White, Color::Reset);
    std::println(stream, "│      {}-v,--version{}             show the program's version", Color::Bold_White, Color::Reset);
    std::println(stream, "│      {}-f,--file{}                specifies the file path", Color::Bold_White, Color::Reset);
    std::println(stream, "│      {}-c,--config{}              specifies the config path", Color::Bold_White, Color::Reset);
    std::println(stream, "│      {}-d,--debug{}               provides more logs", Color::Bold_White, Color::Reset);
    std::println(stream, "│");
    std::println(stream, "╰─{}Version format{}:", Color::Bold_White, Color::Reset);
    std::println(stream, "    {}X{}.{}Y{}.{}Z{}", Color::Bold_Green, Color::Bold_White, Color::Bold_Yellow, Color::Bold_White, Color::Bold_Red, Color::Reset);
    std::println(stream, "    ├─{}X{} - Major version (most stable)", Color::Bold_Green, Color::Reset);
    std::println(stream, "    ├─{}Y{} - Minor version", Color::Bold_Yellow, Color::Reset);
    std::println(stream, "    ╰─{}Z{} - Patch version (least stable)", Color::Bold_Red, Color::Reset);
    exit(EXIT_SUCCESS);
}
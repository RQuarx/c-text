#include <fstream>
#include <print>

#include "../inc/config_parser.hpp"
#include "../inc/utilities.hpp"

#include "../inc/argument_parser.hpp"


ArgParser::ArgParser(int32_t argc, char **argv)
{
    arg_list =
        std::vector<std::string_view>(std::next(argv, 1), std::next(argv, argc));
}


auto
ArgParser::Find_Arg(ArgInput arg) -> bool
{
    return (
        arg.long_arg.empty() ?
        std::ranges::find(arg_list, arg.short_arg) != arg_list.end() :
        std::ranges::find(arg_list, arg.short_arg) != arg_list.end() ||
        std::ranges::find(arg_list, arg.long_arg) != arg_list.end()
    );
}


auto
ArgParser::Option_Arg(std::string &option, ArgInput arg) -> bool
{
    auto iter = std::ranges::find(this->arg_list, arg.short_arg);
    auto iter_one = iter++;

    if (iter != this->arg_list.end() && iter_one != this->arg_list.end()) {
        option = *iter;
        this->arg_list.erase(iter);
        return true;
    }

    if (!arg.long_arg.empty()) {
        iter = std::ranges::find(this->arg_list, arg.long_arg);
        iter_one = iter++;

        if (iter != this->arg_list.end() && iter_one != this->arg_list.end()) {
            this->arg_list.erase(iter);
            option = *iter;
            return true;
        }
    }

    option.clear();
    return false;
}


auto
ArgParser::Get_File_Path(ConfigParser *config, std::string &file_path) -> bool
{
    if (!Option_Arg(file_path, { "-f", "--file" })) {
        std::string back = Back();

        if (back.contains('-')) {
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
    return std::string(arg_list.back());
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
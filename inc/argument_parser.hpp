#pragma once

#include <string>
#include <vector>


struct
ArgInput
{
    std::string_view short_arg;
    std::string_view long_arg;
};


class
ArgParser
{
public:
    ArgParser(int32_t argc, char **argv);

    /// Searches for arg and long arg in the arg list
    /// @param arg a struct containing both the short and long form of the arg
    /// @return true if short arg or long arg is found
    auto Find_Arg(ArgInput arg) -> bool;

    /// Searches for and returns the option for arg and long arg in the arg list
    /// @param option the option param that will be filled
    /// @param arg a struct containing both the short and long form of the arg
    /// @return true and a filled option parameter will be returned if the option is found,
    // else it will return a false and an empty option parameter
    auto Option_Arg(std::string &option, ArgInput arg) -> bool;

    /// Searches and returns a file path to be edited
    /// @param config config parser class
    /// @param file_path a string object that will be filled with the file path
    /// @returns true on empty file, false on filled file
    auto Get_File_Path(class ConfigParser *config, std::string &file_path) -> bool;

    /// Returns the back of the arg_list
    /// @return will return a string of the last arg on the arg_list
    auto Back() -> std::string;

    /// Prints the help message to the specified output stream
    /// @warning this function will exit the program with code EXIT_SUCCESS
    static void Print_Help_Message(FILE *stream);

private:
    std::vector<std::string_view> arg_list;
};


#pragma once

#include <string>
#include <vector>
#include <array>


using sv_pair = std::pair<std::string_view, std::string_view>;

class
ArgParser
{
public:
    ArgParser(int32_t argc, char **argv);

    /// Searches for arg and long arg in the arg list
    /// @param arg a pair containing both the short and long form of the arg
    /// @return true if short arg or long arg is found
    auto Find_Arg(sv_pair arg) -> bool;

    /// Searches for and returns the option for arg and long arg in the arg list
    /// @param option the option param that will be filled
    /// @param arg a pair containing both the short and long form of the arg
    /// @return true and a filled option parameter will be returned if the option is found,
    // else it will return a false and an empty option parameter
    auto Option_Arg(std::string &option, sv_pair arg) -> bool;

    /// Searches and returns a file path to be edited
    /// @param config config parser class
    /// @param file_path a string object that will be filled with the file path
    /// @returns true on empty file, false on filled file
    auto Get_File_Path(class ConfigParser *config, std::string &file_path) -> bool;

    /// Prints the help message to the specified output stream
    /// @warning this function will exit the program with code EXIT_SUCCESS
    static void Print_Help_Message(FILE *stream);

private:
    /// An array of 2 (short, long) containing a vector of a pair,
    //  with a bool which dictated if the string_view is an option or not
    std::array<std::vector<std::pair<bool, std::string_view>>, 2> m_arg_list;
    std::string_view m_bin_path;

    /// Returns the back of the arg_list
    /// @return will return a string of the last arg on the arg_list
    auto Back() -> std::string;

    auto Find_Option_Short(std::string &option, std::string_view short_arg) -> bool;
    auto Find_Option_Long(std::string &option, std::string_view long_arg) -> bool;
};


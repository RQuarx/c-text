#pragma once

#include <cstdio>
#include <string>
#include <print>

#include <SDL3/SDL.h>

#include "../inc/utilities.hpp"


namespace Log {
    static const char *const DEBUG_LABEL = "\e[1;37m[\e[1;36mDEBUG\e[1;37m]:\e[0;0;0m";
    static const char *const INFO_LABEL = "\e[1;37m[\e[1;32mINFO\e[1;37m]:\e[0;0;0m";
    static const char *const ERR_LABEL = "\e[1;37m[\e[1;31mERROR\e[1;37m]:\e[0;0;0m";


    /// Logs SDL errors to stderr
    /// @param fmt format string according to std::format
    /// @param ... additional parameters matching the {} tokens in fmt
    /// @warning this function automatically calls SDL_GetError()
    void
    SDL_Err(std::string_view fmt, auto... args)
    noexcept
    {
        std::string current_time = Utils::Get_Current_Time();
        std::println(
            "\n{} {} {}: {}",
            current_time,
            ERR_LABEL,
            std::vformat(fmt, std::make_format_args(args...)),
            SDL_GetError()
        );
    }


    /// Logs debugs to stream
    /// @param stream output stream of the debug logs
    /// @param fmt format string according to std::format
    /// @param ... additional parameters matching the {} tokens in fmt
    void Debug(FILE *stream, std::string_view fmt, auto... args)
    noexcept
    {
        std::string current_time = Utils::Get_Current_Time();
        std::print(
            stream,
            "{} {} {}",
            current_time,
            DEBUG_LABEL,
            std::vformat(fmt, std::make_format_args(args...))
        );
    }


    /// Logs info to stdout
    /// @param fmt format string according to std::format
    /// @param ... additional parameters matching the {} tokens in fmt
    void
    Info(std::string_view fmt, auto... args)
    noexcept
    {
        std::string current_time = Utils::Get_Current_Time();
        std::print(
            "{} {} {}",
            current_time,
            INFO_LABEL,
            std::vformat(fmt, std::make_format_args(args...))
        );
    }

    /// Logs errors to stderr
    /// @param fmt format string according to std::format
    /// @param ... additional parameters matching the {} tokens in fmt
    void
    Err(std::string_view fmt, auto... args)
    noexcept
    {
        std::string current_time = Utils::Get_Current_Time();
        std::println(
            "\n{} {} {}",
            current_time,
            ERR_LABEL,
            std::vformat(fmt, std::make_format_args(args...))
        );
    }

    /// Prints success and adds a newline
    void Success_Msg();
    /// Prints failed and adds a newline
    void Failed_Msg();
} /* namespace Log */
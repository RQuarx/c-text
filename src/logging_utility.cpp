#include "../inc/logging_utility.hpp"


namespace Log {
    void
    Success_Msg()
    { std::println("{}success{}", Color::Bold_Green, Color::Reset); }


    void
    Failed_Msg()
    { std::println("{}failed{}", Color::Bold_Red, Color::Reset); }
} /* namespace Log */
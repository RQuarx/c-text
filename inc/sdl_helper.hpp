#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include "config_parser.hpp"
#include "utilities.hpp"


struct AppData {
    TTF_TextEngine *text_engine = nullptr;
    const SDL_DisplayMode *display_mode;
    SDL_Renderer *renderer = nullptr;
    SDL_Window *window = nullptr;
    std::unordered_map<std::string_view, TTF_Font*> fonts;

    ConfigParser config;

    bool debug;
};


struct Cache {
    SDL_Surface *surface;
    SDL_Texture *texture;
    std::string_view text;

    [[nodiscard]]
    auto
    Is_Empty() const -> bool
    { return surface == nullptr || texture == nullptr; }

};

struct AppInfo {
    std::string name;
    std::string version;
    std::string identifier;
};

struct WidthAxis {
    int32_t top;
    int32_t right;
    int32_t bottom;
    int32_t left;

    [[nodiscard]]
    auto
    Is_Valid() const -> bool
    { return top >= 0 && right >= 0 && bottom >= 0 && left >= 0; }
};

struct RenderingData {
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color color;
    Position position;


    RenderingData(SDL_Renderer *renderer, SDL_Color color) :
        renderer(renderer),
        font(nullptr),
        color(color) {}

    /// Non text rendering
    RenderingData(SDL_Renderer *renderer, SDL_Color color, Position pos) :
        renderer(renderer),
        font(nullptr),
        color(color),
        position(pos) {}

    /// Text rendering
    RenderingData(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, Position pos) :
        renderer(renderer),
        font(font),
        color(color),
        position(pos) {}
};


class
SDL
{
public:
    /// Initisialises SDL
    /// @returns true on success or false on failure.
    static auto Init(AppData *app_data, AppInfo *app_info, std::string &window_title) -> bool;

    /// Kills SDL
    static void Kill(AppData *app_data);

    /// Draws a circle on a desired point
    /// @param renderer the renderer which should draw the circle
    /// @param center the center point of the circle
    /// @param radius the radius of the circle in pixels
    /// @param color the color of the circle
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Draw_Circle(SDL_Renderer *renderer, SDL_FPoint center, size_t radius, SDL_Color color) -> bool;

    /// Draws a rounded rectangle
    /// @param renderer the renderer which should draw the rectangle
    /// @param rect the base rect
    /// @param radius the radius of the corners
    /// @param color the color of the rectangle
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Draw_Rounded_Rect(SDL_Renderer *renderer, SDL_FRect rect, size_t radius, SDL_Color color) -> bool;

    /// Draws a text to the desired position
    /// @param data struct containing renderer, font, color, and position
    /// @param text the text that will be rendered
    /// @param cache the place in which the texture/surface of the text will be cached on
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Draw_Text(RenderingData data, std::string &text, Cache *cache) -> bool;

    /// Draws a text with a max length
    /// @param data struct containing renderer, font, color, and position
    /// @param max_length_px the max length a string needed to be until cut off
    /// @param text the text that will be rendered
    /// @param cache optional cache
    /// @returns true on success or false on failure.
    static auto Draw_Text_Closed(RenderingData data, int32_t max_length_px, std::string &text, Cache *cache) -> bool;

    /// Get the size of a window's area, in pixels.
    /// @param window the window from which the drawable size should be queried.
    /// @param w a pointer to variable for storing the width in pixels, may be nullptr.
    /// @param h a pointer to variable for storing the height in pixels, may be nullptr.
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Get_Window_Size_Px(SDL_Window *window, int32_t *w, int32_t *h) -> bool;

    /// Calculate the dimensions of a rendered string of UTF-8 text.
    /// This will report the width and height, in pixels, of the space that the specified string will take to fully render.
    /// @param font the font to query.
    /// @param text text to calculate, in UTF-8 encoding.
    /// @param w will be filled with width, in pixels, on return.
    /// @param h will be filled with height, in pixels, on return.
    /// @returns true on success or false on failure.
    /// @warning This function should be called on the thread that created the font.
    static auto Get_String_Size(TTF_Font *font, std::string_view text, int32_t *w, int32_t *h) -> bool;

    /// Set the color used for drawing operations.
    /// Set the color for drawing or filling rectangles, lines, and points, and for SDL_RenderClear().
    /// @param renderer the rendering context.
    /// @param color the color used
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Set_Draw_Color(SDL_Renderer *renderer, SDL_Color color) -> bool;

    /// Set the color used for drawing operations, and makes them blend together.
    /// Set the color for drawing or filling rectangles, lines, and points, and for SDL_RenderClear().
    /// @param renderer the rendering context.
    /// @param color the color used
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Set_Draw_Color_Blend(SDL_Renderer *renderer, SDL_Color color) -> bool;

    /// Inverts color
    /// @param color the color that will be inverted
    /// @returns inverted color
    static auto Invert_Color(SDL_Color color) -> SDL_Color;

    /// Draws a rect outline based on its individual width.
    /// @param data rendering data containing renderer context and color.
    /// @param rect the base rect for the rendering.
    /// @param width the width of each outlines.
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Draw_Rect_Outline(RenderingData data, SDL_FRect rect, WidthAxis width) -> bool;

    /// Calculate the dimensions of a rendered char of UTF-8 text.
    /// This will report the width and height, in pixels, of the space that the specified string will take to fully render.
    /// @param font the font to query.
    /// @param c char to calculate, in UTF-8 encoding.
    /// @param w will be filled with width, in pixels, on return.
    /// @param h will be filled with height, in pixels, on return.
    /// @returns true on success or false on failure.
    /// @warning This function should be called on the thread that created the font.
    static auto Get_Char_Size(TTF_Font *font, char c, int32_t *w, int32_t *h) -> bool;

    /// Fill a rectangle on the current rendering target with the drawing color at subpixel precision.
    /// @param renderer the renderer which should fill a rectangle.
    /// @param rect a pointer to the destination rectangle, or NULL for the entire rendering target.
    /// @returns true on success or false on failure.
    /// @warning This function should only be called on the main thread.
    static auto Draw_Filled_Rect(SDL_Renderer *renderer, SDL_Color color, SDL_FRect *rect) -> bool;

private:
    static auto Init_Window_Renderer(AppData *app_data, const char *window_title, bool debug) -> bool;
    static auto Fetch_Display_Mode(AppData *app_data, bool debug) -> bool;
    static auto Init_App_Metadata(AppInfo *app_info,bool debug) -> bool;
    static auto Load_Fonts(AppData *app_data, bool debug) -> bool;
    static auto Init_Video_Subsystem(bool debug) -> bool;
    static auto Init_SDL_TTF(bool debug) -> bool;
};
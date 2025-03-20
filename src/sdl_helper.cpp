#include <algorithm>

#include "../inc/logging_utility.hpp"
#include "../inc/utilities.hpp"

#include "../inc/sdl_helper.hpp"


auto
SDL::Load_Fonts(AppData *app_data, bool debug) -> bool
{
    if (debug) Log::Debug(stdout, "Loading fonts: ");

    const std::array<std::string_view, 4> font_categories = { "editor", "decoration", "ui", "command" };

    for (auto category : font_categories) {
        const char *font_file = Utils::Match_Font(app_data->config.Get_Value(category, "font"));
        int32_t font_size = app_data->config.Get_Int_Value(category, "font_size");
        app_data->fonts.emplace(category, TTF_OpenFont(font_file, font_size));

        if (app_data->fonts.at(category) == nullptr) {
            if (debug) Log::Failed_Msg();
            Log::SDL_Err("Failed to load font");
            return false;
        }
    }

    if (debug) Log::Success_Msg();
    return true;
}


auto
SDL::Init_Video_Subsystem(bool debug) -> bool
{
    if (debug) Log::Debug(stdout, "Initialising video subsystem: ");
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        if (debug) Log::Failed_Msg();
        Log::SDL_Err("Failed to initialise video subsystem");
        return false;
    }

    if (debug) Log::Success_Msg();
    return true;
}


auto
SDL::Init_App_Metadata(
    AppInfo *app_info,
    bool debug
) -> bool
{
    if (debug) Log::Debug(stdout, "Setting app metadata: ");
    if (
        !SDL_SetAppMetadata(
            app_info->name.c_str(),
            app_info->version.c_str(),
            app_info->identifier.c_str()
        )
    ) {
        if (debug) Log::Failed_Msg();
        Log::Err("Failed to set metadata");
        return false;
    }

    if (debug) Log::Success_Msg();
    return true;
}


auto
SDL::Init_SDL_TTF(bool debug) -> bool
{
    if (debug) Log::Debug(stdout, "Initialising SDL_TTF: ");
    if (!TTF_Init()) {
        if (debug) Log::Failed_Msg();
        Log::Err("Failed to initialise SDL_TTF");
        return false;
    }

    if (debug) Log::Success_Msg();
    return true;
}


auto
SDL::Init_Window_Renderer(AppData *app_data, const char *window_title, bool debug) -> bool
{
    if (debug) Log::Debug(stdout, "Creating window and renderer: ");
    if (
        !SDL_CreateWindowAndRenderer(
            window_title,
            app_data->config.Get_Int_Value("window", "initial_width"),
            app_data->config.Get_Int_Value("window", "initial_height"),
            SDL_WINDOW_HIGH_PIXEL_DENSITY |
            (
                app_data->config.Get_Value("window", "always_floating") == "yes" ?
                0 :
                SDL_WINDOW_RESIZABLE
            ),
            &app_data->window,
            &app_data->renderer
        )
    ) {
        if (debug) Log::Failed_Msg();
        Log::Err("Failed to create window and renderer!");
        return false;
    }

    if (debug) Log::Success_Msg();
    return true;
}


auto
SDL::Fetch_Display_Mode(AppData *app_data, bool debug) -> bool
{
    if (debug) Log::Debug(stdout, "Fetching display mode: ");

    SDL_DisplayID *displays = SDL_GetDisplays(nullptr);
    if (displays == nullptr) {
        if (debug) Log::Failed_Msg();
        Log::Err("Failed to get displays");
        return false;
    }

    app_data->display_mode = SDL_GetCurrentDisplayMode(*displays);
    if (app_data->display_mode == nullptr) {
        if (debug) Log::Failed_Msg();
        Log::Err("Failed to get display mode");
        return false;
    }

    if (debug) Log::Success_Msg();
    return true;
}


auto
SDL::Init(
    AppData *app_data,
    AppInfo *app_info,
    std::string &window_title
) -> bool
{
    if (
        Init_Video_Subsystem(app_data->debug) &&
        Init_App_Metadata(app_info, app_data->debug) &&
        Init_SDL_TTF(app_data->debug) &&
        Load_Fonts(app_data, app_data->debug) &&
        Init_Window_Renderer(app_data, window_title.c_str(), app_data->debug) &&
        Fetch_Display_Mode(app_data, app_data->debug)
    ) {
        if (!SDL_SetRenderVSync(app_data->renderer, 1)) {
            Log::SDL_Err("Failed to set render vsync");
            return false;
        }
        return true;
    }

    return false;
}


void
SDL::Kill(AppData *app_data)
{
    Log::Info("Killing c+text...\n");
    for (auto &font : app_data->fonts) {
        if (font.second != nullptr) TTF_CloseFont(font.second);
    }

    TTF_Quit();
    if (app_data->window != nullptr) SDL_DestroyWindow(app_data->window);
    if (app_data->renderer != nullptr) SDL_DestroyRenderer(app_data->renderer);
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}


auto
SDL::Draw_Circle(SDL_Renderer *renderer, SDL_FPoint center, size_t radius, SDL_Color color) -> bool
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    std::vector<SDL_FPoint> points;

    int32_t x = radius;
    int32_t y = 0;
    int32_t error = 1 - radius;

    while (x >= y) {
        for (int32_t i = -x; i <= x; ++i) {
            points.push_back({ center.x + i, center.y - y });
            points.push_back({ center.x + i, center.y + y });
        }
        for (int32_t i = -y; i <= y; ++i) {
            points.push_back({ center.x + i, center.y - x });
            points.push_back({ center.x + i, center.y + x });
        }

        y++;

        if (error <= 0) { error += 2 * y + 1; }
        else {
            x--;
            error += 2 * (y - x) + 1;
        }
    }

    if (!SDL_RenderPoints(renderer, points.data(), points.size())) {
        Log::SDL_Err("Failed to render point");
        return false;
    }
    return true;
}


auto
SDL::Draw_Rounded_Rect(SDL_Renderer *renderer, SDL_FRect rect, size_t radius, SDL_Color color) -> bool
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    if (radius == 0) {
        if (SDL_RenderFillRect(renderer, &rect)) return true;
        Log::SDL_Err("Failed to render rect");
        return false;
    }

    /* Ensure the radius does not exceed half of the rect's dimension */
    radius = std::min(radius, static_cast<size_t>(rect.w) / 2);
    radius = std::min(radius, static_cast<size_t>(rect.h) / 2);

    std::array<SDL_FRect, 2> rects = {
        {
            { rect.x, rect.y + radius, rect.w, rect.h - (2 * radius) },
            { rect.x + radius, rect.y, rect.w - (2 * radius), rect.h },
        }
    };

    if (!SDL_RenderFillRects(renderer, rects.data(), rects.size())) {
        Log::SDL_Err("Failed to render rect");
        return false;
    }

    /* Drawing the 4 corners */
    std::array<SDL_FPoint, 4> corner_points = {
        {
            { rect.x + radius, rect.y + radius },
            { rect.x + rect.w - radius - 1, rect.y + radius },
            { rect.x + radius, rect.y + radius + rect.h - 1 - (2 * radius) },
            { rect.x + rect.w - radius - 1, rect.y + radius + rect.h - 1 - (2 * radius) }
        }
    };

    if (
        !std::ranges::all_of(
            corner_points,
            [&](const SDL_FPoint& corner_point) {
                return Draw_Circle(renderer, corner_point, radius, color);
            }
        )
    ) {
        Log::Err("Failed to draw corner");
        return false;
    }

    return true;
}


auto
SDL::Draw_Text(
    RenderingData data,
    std::string &text,
    Cache *cache
) -> bool
{
    SDL_Surface *surface = nullptr;
    SDL_Texture *texture = nullptr;

    if (cache == nullptr || cache->Is_Empty()) {
        [[likely]]
        surface = TTF_RenderText_Blended(
            data.font,
            text.empty() ? " " : text.c_str(),
            0,
            data.color
        );
        if (surface == nullptr) {
            Log::SDL_Err("Failed to render text");
            return false;
        }

        texture = SDL_CreateTextureFromSurface(data.renderer, surface);
        if (texture == nullptr) {
            Log::SDL_Err("Failed to create texture");
            SDL_DestroySurface(surface);
            return false;
        }
    } else {
        [[unlikely]]
        surface = cache->surface;
        texture = cache->texture;
    }

    SDL_FRect rect = {
        static_cast<float>(data.position.x),
        static_cast<float>(data.position.y),
        static_cast<float>(surface->w),
        static_cast<float>(surface->h)
    };
    bool return_code =
        SDL_RenderTexture(data.renderer, texture, nullptr, &rect);

    if (cache == nullptr) {
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);
    } else if (cache != nullptr) {
        if (cache->surface != nullptr) SDL_DestroySurface(cache->surface);
        if (cache->texture != nullptr) SDL_DestroyTexture(cache->texture);

        cache->surface = surface;
        cache->texture = texture;
        cache->text = text;
    }

    if (!return_code) Log::SDL_Err("Failed to render texture");
    return return_code;
}


auto
SDL::Draw_Text_Closed(
    RenderingData data,
    int32_t max_length_px,
    std::string &text,
    Cache *cache
) -> bool
{
    int32_t text_length = text.length();
    std::string rendered_text = text;
    int32_t text_width = 0;

    if (max_length_px != 0) {
        if (!TTF_GetStringSize(data.font, text.c_str(), 0, &text_width, nullptr)) {
            Log::SDL_Err("Failed to get string size");
            return false;
        }

        if (text_width > max_length_px) {
            for (int32_t i = text_length; i > 0; i--) {
                rendered_text.resize(i);

                if (!TTF_GetStringSize(data.font, rendered_text.c_str(), 0, &text_width, nullptr)) {
                    Log::SDL_Err("Failed to get string size");
                    return false;
                }

                if (text_width <= max_length_px) break;;
            }
        }

    }

    return Draw_Text(data, rendered_text, cache);
}


auto
SDL::Get_Window_Size_Px(SDL_Window *window, int32_t *w, int32_t *h) -> bool
{
    if (!SDL_GetWindowSizeInPixels(window, w, h)) {
        Log::SDL_Err("Failed to get window size");
        return false;
    }
    return true;
}


auto
SDL::Get_String_Size(TTF_Font *font, std::string_view text, int32_t *w, int32_t *h) -> bool
{
    if (!TTF_GetStringSize(font, std::string(text).c_str(), 0, w, h)) {
        Log::SDL_Err("Failed to get string size");
        return false;
    }
    return true;
}


auto
SDL::Set_Draw_Color(SDL_Renderer *renderer, SDL_Color color) -> bool
{
    if (!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE)) {
        Log::SDL_Err("Failed to set blend mode");
        return false;
    }
    if (!SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a)) {
        Log::SDL_Err("Failed to set draw color");
        return false;
    }
    return true;
}


auto
SDL::Set_Draw_Color_Blend(SDL_Renderer *renderer, SDL_Color color) -> bool
{
    if (!SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND)) {
        Log::SDL_Err("Failed to set blend mode");
        return false;
    }
    if (!SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a)) {
        Log::SDL_Err("Failed to set draw color");
        return false;
    }
    return true;
}


auto
SDL::Invert_Color(SDL_Color color) -> SDL_Color
{
    return {
        static_cast<Uint8>(UINT8_MAX - color.r),
        static_cast<Uint8>(UINT8_MAX - color.g),
        static_cast<Uint8>(UINT8_MAX - color.b),
        static_cast<Uint8>(UINT8_MAX - color.a)
    };
}


auto
SDL::Draw_Rect_Outline(RenderingData data, SDL_FRect rect, WidthAxis width) -> bool
{
    if (!width.Is_Valid()) {
        Log::Err("Invalid outline width: {}" "Outline width is less than 0.");
        return false;
    }

    if (!Set_Draw_Color_Blend(data.renderer, data.color)) return false;

    /* Top, Right, Bottom, Left */
    std::array<SDL_FRect, 4> rects = {
        {
            { rect.x, rect.y - width.top, rect.w, static_cast<float>(width.top) },
            { rect.x + rect.w, rect.y, static_cast<float>(width.right), rect.h },
            { rect.x, rect.y + rect.h, rect.w, static_cast<float>(width.bottom) },
            { rect.x - width.left, rect.y, static_cast<float>(width.left), rect.h },
        }
    };

    if (!SDL_RenderFillRects(data.renderer, rects.data(), rects.size())) {
        Log::SDL_Err("Failed to render outlines");
        return false;
    }
    return true;
}


auto
SDL::Get_Char_Size(TTF_Font *font, char c, int32_t *w, int32_t *h) -> bool
{
    return Get_String_Size(font, std::string(1, c), w, h);
}


auto
SDL::Draw_Filled_Rect(SDL_Renderer *renderer, SDL_Color color, SDL_FRect *rect) -> bool
{
    if (!Set_Draw_Color_Blend(renderer, color)) return false;
    if (!SDL_RenderFillRect(renderer, rect)) {
        Log::SDL_Err("Failed to render rect");
        return false;
    }
    return true;
}
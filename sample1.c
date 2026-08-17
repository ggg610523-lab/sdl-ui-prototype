#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "uikit.h"

/*
 * Separated UI components.
 *
 * These are header-only implementations and therefore must be included
 * in the translation unit that uses them.
 */
#include "label.h"
#include "button.h"
#include "slider.h"
#include "toggle.h"
#include "checkbox.h"
#include "radio.h"
#include "panel.h"


#define WINDOW_W 1180
#define WINDOW_H 760


static void print_sdl_error(const char *where)
{
    fprintf(stderr, "%s: %s\n", where, SDL_GetError());
}


int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        print_sdl_error("SDL_Init failed");
        return 1;
    }

    if (!TTF_Init()) {
        print_sdl_error("TTF_Init failed");
        SDL_Quit();
        return 1;
    }

    /*
     * MuternVF.ttf
     *
     * Expected:
     *
     * assets/
     *     MuternVF.ttf
     */
    const char *font_path = "assets/MuternVF.ttf";

    TTF_Font *font = TTF_OpenFont(font_path, 16.0f);

    if (!font) {
        fprintf(
            stderr,
            "Could not open font '%s': %s\n",
            font_path,
            SDL_GetError()
        );

        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    /*
     * Extra sizes.
     *
     * Mutern is used throughout the UI so the typography stays consistent.
     */
    TTF_Font *font_sm = TTF_OpenFont(font_path, 13.0f);
    TTF_Font *font_lg = TTF_OpenFont(font_path, 23.0f);
    TTF_Font *font_xs = TTF_OpenFont(font_path, 11.0f);

    if (!font_sm || !font_lg || !font_xs) {
        fprintf(stderr, "Could not open UI font variants: %s\n",
                SDL_GetError());

        if (font_sm) TTF_CloseFont(font_sm);
        if (font_lg) TTF_CloseFont(font_lg);
        if (font_xs) TTF_CloseFont(font_xs);

        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }


    SDL_Window *window = SDL_CreateWindow(
        "macOS 26 UI",
        WINDOW_W,
        WINDOW_H,
        SDL_WINDOW_RESIZABLE
    );

    if (!window) {
        print_sdl_error("SDL_CreateWindow failed");

        TTF_CloseFont(font_xs);
        TTF_CloseFont(font_lg);
        TTF_CloseFont(font_sm);
        TTF_CloseFont(font);

        TTF_Quit();
        SDL_Quit();
        return 1;
    }


    SDL_Renderer *renderer = SDL_CreateRenderer(
        window,
        NULL
    );

    if (!renderer) {
        print_sdl_error("SDL_CreateRenderer failed");

        SDL_DestroyWindow(window);

        TTF_CloseFont(font_xs);
        TTF_CloseFont(font_lg);
        TTF_CloseFont(font_sm);
        TTF_CloseFont(font);

        TTF_Quit();
        SDL_Quit();
        return 1;
    }


    /*
     * UI core.
     */
    UK uk = uk_make(renderer, font);

    /*
     * The core's default font is the regular Mutern font.
     * Override the other sizes here.
     */
    uk.font = font;
    uk.font_sm = font_sm;
    uk.font_lg = font_lg;
    uk.font_xs = font_xs;


    /*
     * Application state.
     */
    bool running = true;

    bool wifi_enabled = true;
    bool bluetooth_enabled = false;

    bool dark_mode = true;

    bool check_a = true;
    bool check_b = false;

    bool radio_a = true;
    bool radio_b = false;
    bool radio_c = false;

    float slider_value = 0.68f;

    Uint64 last_counter = SDL_GetPerformanceCounter();


    while (running) {

        /*
         * Timing.
         */
        Uint64 now = SDL_GetPerformanceCounter();

        float dt =
            (float)(
                (double)(now - last_counter) /
                (double)SDL_GetPerformanceFrequency()
            );

        last_counter = now;

        if (dt > 0.05f)
            dt = 0.05f;


        /*
         * Window size.
         */
        int sw = 0;
        int sh = 0;

        SDL_GetWindowSize(
            window,
            &sw,
            &sh
        );


        /*
         * Mouse state.
         */
        float mx = 0.0f;
        float my = 0.0f;

        SDL_GetMouseState(
            &mx,
            &my
        );


        bool mouse_down = false;
        bool mouse_press = false;
        bool mouse_release = false;


        /*
         * Events.
         */
        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            switch (event.type) {

                case SDL_EVENT_QUIT:
                    running = false;
                    break;


                case SDL_EVENT_MOUSE_BUTTON_DOWN:

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        mouse_down = true;
                        mouse_press = true;
                    }

                    break;


                case SDL_EVENT_MOUSE_BUTTON_UP:

                    if (event.button.button == SDL_BUTTON_LEFT) {
                        mouse_down = false;
                        mouse_release = true;
                    }

                    break;


                case SDL_EVENT_KEY_DOWN:

                    if (event.key.key == SDLK_ESCAPE) {
                        running = false;
                    }

                    break;

                default:
                    break;
            }
        }


        /*
         * Preserve held mouse state from SDL.
         *
         * This makes dragging controls work even though the button-down
         * event itself only happens for one frame.
         */
        SDL_MouseButtonFlags mouse_buttons =
            SDL_GetMouseState(NULL, NULL);

        if (mouse_buttons & SDL_BUTTON_LMASK)
            mouse_down = true;


        /*
         * Begin UI frame.
         */
        uk_begin(
            &uk,
            mx,
            my,
            mouse_down,
            mouse_press,
            mouse_release,
            dt,
            sw,
            sh
        );


        /*
         * ================================================================
         * BACKGROUND
         * ================================================================
         *
         * No light-following renderer.
         *
         * The UI is deliberately dark and static, with restrained
         * translucent surfaces rather than glowing gradients.
         */
        uk_bg_draw(&uk);


        /*
         * ================================================================
         * MAIN WINDOW / SIDEBAR
         * ================================================================
         */

        float margin = 34.0f;
        float sidebar_w = 215.0f;

        float content_x = margin + sidebar_w + 28.0f;
        float content_w = (float)sw - content_x - margin;


        /*
         * Sidebar.
         */
        uk_glass_panel_begin(
            &uk,
            margin,
            margin,
            sidebar_w,
            (float)sh - margin * 2.0f
        );


        /*
         * App title.
         */
        uk_set_pos(
            &uk,
            margin + 22.0f,
            margin + 22.0f
        );

        uk_set_width(
            &uk,
            sidebar_w - 44.0f
        );


        uk_heading(
            &uk,
            "System"
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_label_sm(
            &uk,
            "Settings"
        );

        uk_spacing(
            &uk,
            18.0f
        );


        /*
         * Sidebar buttons.
         */
        uk_button(
            &uk,
            100,
            "General",
            38.0f
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_button_ghost(
            &uk,
            101,
            "Appearance",
            38.0f
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_button_ghost(
            &uk,
            102,
            "Control Center",
            38.0f
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_button_ghost(
            &uk,
            103,
            "Desktop & Dock",
            38.0f
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_button_ghost(
            &uk,
            104,
            "Displays",
            38.0f
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_button_ghost(
            &uk,
            105,
            "Sound",
            38.0f
        );


        /*
         * Bottom sidebar status.
         */
        uk_set_pos(
            &uk,
            margin + 22.0f,
            (float)sh - margin - 76.0f
        );

        uk_set_width(
            &uk,
            sidebar_w - 44.0f
        );

        uk_label_sm(
            &uk,
            "This Mac"
        );

        uk_spacing(
            &uk,
            4.0f
        );

        uk_label_color(
            &uk,
            "Online",
            uk.th.success
        );


        uk_glass_panel_end(&uk);


        /*
         * ================================================================
         * CONTENT
         * ================================================================
         */

        uk_set_pos(
            &uk,
            content_x,
            margin + 18.0f
        );

        uk_set_width(
            &uk,
            content_w
        );


        /*
         * Header.
         */
        uk_heading(
            &uk,
            "General"
        );

        uk_spacing(
            &uk,
            4.0f
        );

        uk_label_sm(
            &uk,
            "Manage system-wide settings and preferences."
        );

        uk_spacing(
            &uk,
            22.0f
        );


        /*
         * ================================================================
         * CONNECTION PANEL
         * ================================================================
         */

        uk_glass_panel_begin(
            &uk,
            content_x,
            margin + 102.0f,
            content_w,
            156.0f
        );


        uk_set_pos(
            &uk,
            content_x + 22.0f,
            margin + 122.0f
        );

        uk_set_width(
            &uk,
            content_w - 44.0f
        );


        uk_label(
            &uk,
            "Connectivity"
        );

        uk_spacing(
            &uk,
            8.0f
        );

        uk_label_sm(
            &uk,
            "Wireless connections and nearby devices."
        );

        uk_spacing(
            &uk,
            16.0f
        );


        uk_toggle(
            &uk,
            201,
            "Wi-Fi",
            &wifi_enabled
        );

        uk_spacing(
            &uk,
            7.0f
        );

        uk_toggle(
            &uk,
            202,
            "Bluetooth",
            &bluetooth_enabled
        );


        uk_glass_panel_end(&uk);


        /*
         * ================================================================
         * APPEARANCE PANEL
         * ================================================================
         */

        float panel_y = margin + 278.0f;

        uk_glass_panel_begin(
            &uk,
            content_x,
            panel_y,
            content_w,
            205.0f
        );


        uk_set_pos(
            &uk,
            content_x + 22.0f,
            panel_y + 20.0f
        );

        uk_set_width(
            &uk,
            content_w - 44.0f
        );


        uk_label(
            &uk,
            "Appearance"
        );

        uk_spacing(
            &uk,
            6.0f
        );

        uk_label_sm(
            &uk,
            "Choose how the interface should appear."
        );

        uk_spacing(
            &uk,
            16.0f
        );


        /*
         * Dark mode.
         */
        uk_toggle(
            &uk,
            301,
            "Dark appearance",
            &dark_mode
        );

        uk_spacing(
            &uk,
            13.0f
        );


        /*
         * Slider.
         */
        uk_label_sm(
            &uk,
            "Interface intensity"
        );

        uk_spacing(
            &uk,
            6.0f
        );

        slider_value = uk_slider(
            &uk,
            302,
            slider_value
        );


        uk_glass_panel_end(&uk);


        /*
         * ================================================================
         * OPTIONS
         * ================================================================
         */

        float options_y = panel_y + 221.0f;

        uk_glass_panel_begin(
            &uk,
            content_x,
            options_y,
            content_w,
            185.0f
        );


        uk_set_pos(
            &uk,
            content_x + 22.0f,
            options_y + 20.0f
        );

        uk_set_width(
            &uk,
            content_w - 44.0f
        );


        uk_label(
            &uk,
            "Options"
        );

        uk_spacing(
            &uk,
            14.0f
        );


        uk_checkbox(
            &uk,
            401,
            "Show controls in menu bar",
            &check_a
        );

        uk_spacing(
            &uk,
            8.0f
        );

        uk_checkbox(
            &uk,
            402,
            "Reduce transparency",
            &check_b
        );

        uk_spacing(
            &uk,
            14.0f
        );


        uk_label_sm(
            &uk,
            "Accent color"
        );

        uk_spacing(
            &uk,
            7.0f
        );


        uk_radio(
            &uk,
            411,
            "Blue",
            radio_a
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_radio(
            &uk,
            412,
            "Purple",
            radio_b
        );

        uk_spacing(
            &uk,
            5.0f
        );

        uk_radio(
            &uk,
            413,
            "Orange",
            radio_c
        );


        uk_glass_panel_end(&uk);


        /*
         * ================================================================
         * FOOTER
         * ================================================================
         */

        float footer_y =
            (float)sh - margin - 38.0f;


        uk_set_pos(
            &uk,
            content_x,
            footer_y
        );

        uk_set_width(
            &uk,
            content_w
        );


        uk_label_color(
            &uk,
            "Changes are saved automatically.",
            uk.th.text_dim
        );


        /*
         * Finish frame.
         */
        uk_end(&uk);


        /*
         * Present.
         */
        SDL_RenderPresent(renderer);
    }


    /*
     * Cleanup.
     */
    uk_shutdown(&uk);

    TTF_CloseFont(font_xs);
    TTF_CloseFont(font_lg);
    TTF_CloseFont(font_sm);
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}

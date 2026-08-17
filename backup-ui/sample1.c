#include <stdio.h>
#include <math.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "uikit.h"
#include "label.h"
#include "button.h"
#include "slider.h"
#include "toggle.h"
#include "radio.h"
#include "checkbox.h"
#include "panel.h"

static float clampf(float v, float a, float b)
{
    if (v < a) return a;
    if (v > b) return b;
    return v;
}

int main(void)
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n",
                SDL_GetError());
        return 1;
    }

    if (!TTF_Init()) {
        fprintf(stderr, "TTF_Init failed: %s\n",
                SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow(
            "UIKit — Liquid Glass",
            900,
            760,
            SDL_WINDOW_RESIZABLE |
            SDL_WINDOW_HIGH_PIXEL_DENSITY
        );

    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n",
                SDL_GetError());

        TTF_Quit();
        SDL_Quit();

        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(
            window,
            NULL
        );

    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n",
                SDL_GetError());

        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();

        return 1;
    }

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    TTF_Font *font =
        TTF_OpenFont(
            "assets/MuternVF.ttf",
            15
        );

    TTF_Font *font_sm =
        TTF_OpenFont(
            "assets/MuternVF.ttf",
            12
        );

    TTF_Font *font_lg =
        TTF_OpenFont(
            "assets/MuternVF.ttf",
            21
        );

    TTF_Font *font_xs =
        TTF_OpenFont(
            "assets/MuternVF.ttf",
            10
        );

    if (!font || !font_sm || !font_lg || !font_xs) {

        fprintf(
            stderr,
            "Could not load assets/MuternVF.ttf\n"
        );

        if (font) TTF_CloseFont(font);
        if (font_sm) TTF_CloseFont(font_sm);
        if (font_lg) TTF_CloseFont(font_lg);
        if (font_xs) TTF_CloseFont(font_xs);

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        TTF_Quit();
        SDL_Quit();

        return 1;
    }

    UK uk =
        uk_make(renderer, font);

    uk.font_sm = font_sm;
    uk.font_lg = font_lg;
    uk.font_xs = font_xs;

    float volume = 0.62f;
    float brightness = 0.38f;
    float speed = 75.0f;

    bool dark = true;
    bool notifications = true;
    bool haptic = true;

    bool feature_a = true;
    bool feature_b = false;
    bool feature_c = true;

    int radio = 1;

    bool running = true;

    Uint64 previous =
        SDL_GetTicks();

    while (running) {

        Uint64 now =
            SDL_GetTicks();

        float dt =
            (float)(now - previous) /
            1000.0f;

        previous = now;

        dt = clampf(
            dt,
            0.0f,
            0.05f
        );

        int output_w;
        int output_h;

        SDL_GetRenderOutputSize(
            renderer,
            &output_w,
            &output_h
        );

        /*
           SDL mouse coordinates are window coordinates.

           The renderer can be larger because of
           HIGH_PIXEL_DENSITY, so convert them into
           renderer coordinates.
        */

        int window_w;
        int window_h;

        SDL_GetWindowSize(
            window,
            &window_w,
            &window_h
        );

        float dpi_x =
            window_w > 0 ?
            (float)output_w / window_w :
            1.0f;

        float dpi_y =
            window_h > 0 ?
            (float)output_h / window_h :
            1.0f;

        float mouse_x = 0;
        float mouse_y = 0;

        bool mouse_down = false;
        bool mouse_pressed = false;
        bool mouse_released = false;

        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            switch (event.type) {

            case SDL_EVENT_QUIT:
                running = false;
                break;

            case SDL_EVENT_KEY_DOWN:

                if (event.key.key == SDLK_ESCAPE)
                    running = false;

                break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:

                if (event.button.button ==
                    SDL_BUTTON_LEFT) {

                    mouse_x =
                        event.button.x *
                        dpi_x;

                    mouse_y =
                        event.button.y *
                        dpi_y;

                    mouse_down = true;
                    mouse_pressed = true;
                }

                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:

                if (event.button.button ==
                    SDL_BUTTON_LEFT) {

                    mouse_x =
                        event.button.x *
                        dpi_x;

                    mouse_y =
                        event.button.y *
                        dpi_y;

                    mouse_down = false;
                    mouse_released = true;
                }

                break;

            case SDL_EVENT_MOUSE_MOTION:

                mouse_x =
                    event.motion.x *
                    dpi_x;

                mouse_y =
                    event.motion.y *
                    dpi_y;

                mouse_down =
                    (event.motion.state &
                     SDL_BUTTON_LMASK) != 0;

                break;

            default:
                break;
            }
        }

        uk_begin(
            &uk,
            mouse_x,
            mouse_y,
            mouse_down,
            mouse_pressed,
            mouse_released,
            dt,
            output_w,
            output_h
        );

        /* background */

        uk_bg_draw(&uk);

        /*
           Everything below this point uses flow layout.

           Every widget advances uk.cy.
           Nothing uses hard-coded absolute Y positions.
        */

        float s = uk.scale;

        uk_set_pos(
            &uk,
            uk.root_x,
            uk.root_y
        );

        uk_set_width(
            &uk,
            uk.root_w
        );

        /*
           Header
        */

        uk_heading(
            &uk,
            "Liquid Glass"
        );

        uk_label_sm(
            &uk,
            "Native SDL3 interface with responsive glass surfaces"
        );

        uk_spacing(
            &uk,
            12
        );

        /*
           Controls panel
        */

        float panel_h =
            180.0f * s;

        uk_glass_panel_begin(
            &uk,
            panel_h
        );

        uk_heading(
            &uk,
            "Controls"
        );

        uk_slider(
            &uk,
            &volume,
            0.0f,
            1.0f,
            "Volume"
        );

        uk_slider(
            &uk,
            &brightness,
            0.0f,
            1.0f,
            "Brightness"
        );

        uk_slider(
            &uk,
            &speed,
            0.0f,
            100.0f,
            "Speed"
        );

        uk_glass_panel_end(&uk);

        /*
           Buttons
        */

        uk_spacing(&uk, 8);

        uk_glass_panel_begin(
            &uk,
            194.0f * s
        );

        uk_heading(
            &uk,
            "Buttons"
        );

        if (uk_button(
            &uk,
            "Glass Button"))
            printf("glass button\n");

        if (uk_button_accent(
            &uk,
            "Accent Glass"))
            printf("accent\n");

        if (uk_button_ghost(
            &uk,
            "Ghost"))
            printf("ghost\n");

        uk_glass_panel_end(&uk);

        /*
           Switches
        */

        uk_spacing(&uk, 8);

        uk_glass_panel_begin(
            &uk,
            148.0f * s
        );

        uk_heading(
            &uk,
            "Appearance"
        );

        uk_toggle(
            &uk,
            &dark,
            "Dark Mode"
        );

        uk_toggle(
            &uk,
            &notifications,
            "Notifications"
        );

        uk_toggle(
            &uk,
            &haptic,
            "Haptic Feedback"
        );

        uk_glass_panel_end(&uk);

        /*
           Selection
        */

        uk_spacing(&uk, 8);

        uk_glass_panel_begin(
            &uk,
            190.0f * s
        );

        uk_heading(
            &uk,
            "Selection"
        );

        uk_checkbox(
            &uk,
            &feature_a,
            "Feature A"
        );

        uk_checkbox(
            &uk,
            &feature_b,
            "Feature B"
        );

        uk_checkbox(
            &uk,
            &feature_c,
            "Feature C"
        );

        uk_spacing(&uk, 4);

        const char *modes[] = {
            "Compact",
            "Default",
            "Comfortable"
        };

        uk_radio(
            &uk,
            &radio,
            modes,
            3
        );

        uk_glass_panel_end(&uk);

        uk_spacing(&uk, 25);

        uk_label_sm(
            &uk,
            "Resize the window — the layout scales automatically."
        );

        uk_end(&uk);

        SDL_RenderPresent(
            renderer
        );

        SDL_Delay(4);
    }

    uk_shutdown(&uk);

    TTF_CloseFont(font);
    TTF_CloseFont(font_sm);
    TTF_CloseFont(font_lg);
    TTF_CloseFont(font_xs);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();

    return 0;
}

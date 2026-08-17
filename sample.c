#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <math.h>

#include "uikit.h"
#include "label.h"
#include "button.h"
#include "slider.h"
#include "toggle.h"
#include "radio.h"
#include "checkbox.h"
#include "panel.h"

static float clampf(float v, float lo, float hi) {
    return v < lo ? lo : v > hi ? hi : v;
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *win = SDL_CreateWindow("UIKit — Liquid Glass",
        800, 700, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
    SDL_Renderer *ren = SDL_CreateRenderer(win, NULL);
    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    TTF_Font *font    = TTF_OpenFont("assets/MuternVF.ttf", 15);
    TTF_Font *font_sm = TTF_OpenFont("assets/MuternVF.ttf", 12);
    TTF_Font *font_lg = TTF_OpenFont("assets/MuternVF.ttf", 20);
    TTF_Font *font_xs = TTF_OpenFont("assets/MuternVF.ttf", 10);

    if (!font || !font_sm || !font_lg || !font_xs) {
        printf("Failed to load fonts\n");
        return 1;
    }

    UK uk = uk_make(ren, font);
    uk.font_sm = font_sm;
    uk.font_lg = font_lg;
    uk.font_xs = font_xs;

    float vol = 0.6f, bright = 0.35f, speed = 0.8f;
    bool dark = true, notif = false, haptic = true;
    int radio_sel = 0;
    bool check_a = true, check_b = false, check_c = true;

    bool running = true;
    Uint64 prev = SDL_GetTicks();

    while (running) {
        Uint64 now = SDL_GetTicks();
        float dt = (float)(now - prev) / 1000.f;
        prev = now;
        dt = clampf(dt, 0, 0.05f);

        int ow, oh;
        SDL_GetRenderOutputSize(ren, &ow, &oh);

        bool mpressed = false, mreleased = false, mdown = false;
        float mx = 0, my = 0;
        SDL_Event ev;
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_EVENT_QUIT) running = false;
            if (ev.type == SDL_EVENT_KEY_DOWN && ev.key.key == SDLK_ESCAPE)
                running = false;
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN && ev.button.button == SDL_BUTTON_LEFT) {
                mpressed = true; mdown = true; mx = ev.button.x; my = ev.button.y;
            }
            if (ev.type == SDL_EVENT_MOUSE_BUTTON_UP && ev.button.button == SDL_BUTTON_LEFT) {
                mreleased = true; mdown = false; mx = ev.button.x; my = ev.button.y;
            }
            if (ev.type == SDL_EVENT_MOUSE_MOTION) {
                mdown = (ev.motion.state & SDL_BUTTON_LMASK) != 0;
                mx = ev.motion.x; my = ev.motion.y;
            }
        }

        uk_begin(&uk, mx, my, mdown, mpressed, mreleased, dt, ow, oh);

        /* rich background */
        uk_bg_draw(&uk);

        /* centered content */
        float s = uk.scale;
        float max_w = 440;
        float cw = fminf((float)ow - 40*s, max_w * s);
        float ox = ((float)ow - cw) * 0.5f;
        float oy = 24 * s;
        uk_set_pos(&uk, ox, oy);
        uk_set_width(&uk, cw);

        /* title */
        uk_heading(&uk, "UIKit");
        uk_label_sm(&uk, "Liquid glass UI for SDL3");
        uk_spacing(&uk, 10);

        /* ---- SLIDERS ---- */
        {
            float ph = 3 * 38 * s;
            UK_Panel p = uk_panel_begin(&uk, ph);
            uk_slider(&uk, &vol, 0, 1, "Volume");
            uk_slider(&uk, &bright, 0, 1, "Brightness");
            uk_slider(&uk, &speed, 0, 100, "Speed");
            uk_panel_end(&uk, &p);
        }

        /* ---- BUTTONS ---- */
        {
            float ph = 3 * 44 * s;
            UK_Panel p = uk_panel_begin(&uk, ph);
            if (uk_button(&uk, "Default")) printf("default\n");
            if (uk_button_accent(&uk, "Accent")) printf("accent\n");
            if (uk_button_ghost(&uk, "Ghost")) printf("ghost\n");
            uk_panel_end(&uk, &p);
        }

        /* ---- TOGGLES ---- */
        {
            float ph = 3 * 34 * s;
            UK_Panel p = uk_panel_begin(&uk, ph);
            uk_toggle(&uk, &dark, "Dark Mode");
            uk_toggle(&uk, &notif, "Notifications");
            uk_toggle(&uk, &haptic, "Haptic Feedback");
            uk_panel_end(&uk, &p);
        }

        /* ---- CHECKBOXES ---- */
        {
            float ph = 3 * 30 * s;
            UK_Panel p = uk_panel_begin(&uk, ph);
            uk_checkbox(&uk, &check_a, "Feature A");
            uk_checkbox(&uk, &check_b, "Feature B");
            uk_checkbox(&uk, &check_c, "Feature C");
            uk_panel_end(&uk, &p);
        }

        /* ---- RADIOS ---- */
        {
            float ph = 3 * 32 * s;
            UK_Panel p = uk_panel_begin(&uk, ph);
            const char *opts[] = { "Compact", "Default", "Comfortable" };
            int r = uk_radio(&uk, &radio_sel, opts, 3);
            if (r >= 0) printf("radio: %s\n", opts[r]);
            uk_panel_end(&uk, &p);
        }

        /* ---- LABELS ---- */
        {
            float ph = 3 * 26 * s;
            UK_Panel p = uk_panel_begin(&uk, ph);
            uk_label(&uk, "Default label");
            uk_label_sm(&uk, "Small dim label");
            uk_label_color(&uk, "Accent label", uk.th.accent);
            uk_panel_end(&uk, &p);
        }

        uk_spacing(&uk, 40);

        uk_end(&uk);
        SDL_RenderPresent(ren);
        SDL_Delay(4);
    }

    TTF_CloseFont(font);
    TTF_CloseFont(font_sm);
    TTF_CloseFont(font_lg);
    TTF_CloseFont(font_xs);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

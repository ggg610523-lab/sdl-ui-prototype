#ifndef UKIT_H
#define UKIT_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#define UK_STATE_MAX 256

typedef struct {
    float r, g, b, a;
} UK_Color;

typedef struct {
    float x, y, w, h;
} UK_Rect;


/* ============================================================
   THEME
   ============================================================ */

typedef struct {

    /* Background */
    UK_Color bg_top;
    UK_Color bg_bot;

    UK_Color orb1;
    UK_Color orb2;
    UK_Color orb3;
    UK_Color orb4;

    /* Text */
    UK_Color text;
    UK_Color text_dim;
    UK_Color text_bright;

    /* Accent */
    UK_Color accent;
    UK_Color accent2;
    UK_Color success;
    UK_Color danger;

    /* Liquid glass */
    UK_Color glass_bg;
    UK_Color glass_border;
    UK_Color glass_highlight;
    UK_Color glass_shadow;
    UK_Color glass_tint;

    /* Sliders */
    UK_Color slider_track;
    UK_Color slider_fill;
    UK_Color slider_knob;

    /* Toggles */
    UK_Color toggle_off_bg;
    UK_Color toggle_on_bg;
    UK_Color toggle_knob;

    /* Radios */
    UK_Color radio_off_bg;
    UK_Color radio_on_bg;
    UK_Color radio_knob;

    /* Checkboxes */
    UK_Color check_bg;
    UK_Color check_on;
    UK_Color check_knob;

    /* Buttons */
    UK_Color btn_bg;
    UK_Color btn_hover;
    UK_Color btn_active;
    UK_Color btn_text;

    /* Compatibility names used by newer uikit.c */
    UK_Color button;
    UK_Color button_pressed;
    UK_Color button_text;

    float corner;
    float spacing;

} UK_Theme;


/* ============================================================
   STATE
   ============================================================ */

typedef struct {
    float anim;
    float drag_val;
    bool dragging;
} UK_WidgetState;

typedef struct {
    UK_WidgetState states[UK_STATE_MAX];
} UK_StatePool;


/* ============================================================
   UI CONTEXT
   ============================================================ */

typedef struct {

    SDL_Renderer *ren;

    TTF_Font *font;
    TTF_Font *font_sm;
    TTF_Font *font_lg;
    TTF_Font *font_xs;

    UK_Theme th;
    UK_StatePool pool;

    /* Mouse */
    float mx;
    float my;

    bool mdown;
    bool mpress;
    bool mrelease;

    /* Timing */
    float dt;
    float time;

    /* Current layout cursor */
    float cx;
    float cy;
    float cw;

    /* Root layout */
    float root_x;
    float root_y;
    float root_w;
    float root_h;

    /* Window */
    int win_w;
    int win_h;

    /* Responsive scale */
    float scale;

} UK;


/* ============================================================
   CORE
   ============================================================ */

UK uk_make(
    SDL_Renderer *ren,
    TTF_Font *font
);

void uk_begin(
    UK *uk,
    float mx,
    float my,
    bool mdown,
    bool mpress,
    bool mrelease,
    float dt,
    int sw,
    int sh
);

void uk_end(
    UK *uk
);

void uk_shutdown(
    UK *uk
);


/* ============================================================
   LAYOUT
   ============================================================ */

void uk_set_pos(
    UK *uk,
    float x,
    float y
);

void uk_set_width(
    UK *uk,
    float w
);

void uk_indent(
    UK *uk,
    float dx
);

/*
 * Component headers already define this as:
 *
 *     static inline float uk_spacing(...)
 *
 * Therefore it MUST NOT be declared here.
 */


/* ============================================================
   SCALE
   ============================================================ */

float uk_scale(
    UK *uk,
    float value
);


/* ============================================================
   COLORS
   ============================================================ */

UK_Color uk_rgba(
    float r,
    float g,
    float b,
    float a
);

static inline UK_Color uk_color_lerp(
    UK_Color a,
    UK_Color b,
    float t
) {
    return (UK_Color){
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t
    };
}


/* ============================================================
   STATE
   ============================================================ */

UK_WidgetState *uk_state(
    UK *uk,
    unsigned id
);


/* ============================================================
   BACKGROUND
   ============================================================ */

void uk_bg_render(
    UK *uk
);

void uk_bg_draw(
    UK *uk
);


/* ============================================================
   DRAWING
   ============================================================ */

void uk_rect_fill(
    UK *uk,
    UK_Rect r,
    UK_Color c
);

void uk_rect_fill_r(
    UK *uk,
    UK_Rect r,
    float radius,
    UK_Color c
);

void uk_rect_stroke_r(
    UK *uk,
    UK_Rect r,
    float radius,
    UK_Color c,
    float thickness
);

void uk_circle(
    UK *uk,
    float cx,
    float cy,
    float radius,
    UK_Color c
);

void uk_line(
    UK *uk,
    float x0,
    float y0,
    float x1,
    float y1,
    UK_Color c,
    float thickness
);


/* ============================================================
   LIQUID GLASS
   ============================================================ */

void uk_glass_begin(
    UK *uk,
    float x,
    float y,
    float w,
    float h
);

void uk_glass_end(
    UK *uk,
    float x,
    float y,
    float w,
    float h
);

void uk_glass_surface(
    UK *uk,
    UK_Rect r
);


/* ============================================================
   TEXT
   ============================================================ */

float uk_text_w(
    TTF_Font *font,
    const char *text
);

int uk_text_h(
    TTF_Font *font
);

void uk_text(
    UK *uk,
    const char *text,
    float x,
    float y,
    UK_Color color
);

void uk_text_font(
    UK *uk,
    TTF_Font *font,
    const char *text,
    float x,
    float y,
    UK_Color color
);


/*
 * DO NOT declare:
 *
 * uk_heading
 * uk_label
 * uk_label_sm
 * uk_label_color
 * uk_spacing
 *
 * here.
 *
 * Those are already static-inline functions in label.h.
 */


/* ============================================================
   HELPERS
   ============================================================ */

static inline float uk_clampf(
    float v,
    float lo,
    float hi
) {
    return v < lo ? lo :
           v > hi ? hi :
           v;
}

#endif

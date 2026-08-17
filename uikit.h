#ifndef UIKIT_H
#define UIKIT_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UK_STATE_MAX 512

typedef struct {
    float r, g, b, a;
} UK_Color;

typedef struct {
    float x, y, w, h;
} UK_Rect;

typedef struct {
    float value;
    float velocity;
    float anim;
    bool dragging;
    bool initialized;
    bool hovered;
    bool pressed;
} UK_WidgetState;

/*
 * Canonical theme.
 *
 * The aliases at the bottom intentionally remain here so older sample
 * programs using names such as text_dim, btn_bg, etc. continue compiling.
 */
typedef struct {

    /* Window / content */
    UK_Color background;
    UK_Color background_secondary;
    UK_Color content;
    UK_Color content_secondary;

    /* Text */
    UK_Color text;
    UK_Color text_secondary;
    UK_Color text_tertiary;
    UK_Color text_dim;
    UK_Color text_bright;

    /* Accent */
    UK_Color accent;
    UK_Color accent2;
    UK_Color accent_hover;
    UK_Color accent_pressed;

    UK_Color success;
    UK_Color danger;
    UK_Color warning;

    /* Material surfaces */
    UK_Color material_thin;
    UK_Color material_regular;
    UK_Color material_thick;

    UK_Color panel;
    UK_Color panel_border;
    UK_Color panel_highlight;

    UK_Color glass_bg;
    UK_Color glass_border;
    UK_Color glass_highlight;
    UK_Color glass_shadow;
    UK_Color glass_tint;

    /* Buttons */
    UK_Color button;
    UK_Color button_hover;
    UK_Color button_pressed;
    UK_Color button_text;

    /* Compatibility aliases */
    UK_Color btn_bg;
    UK_Color btn_hover;
    UK_Color btn_active;
    UK_Color btn_text;

    /* Slider */
    UK_Color slider_track;
    UK_Color slider_fill;
    UK_Color slider_knob;

    /* Toggle */
    UK_Color toggle_off;
    UK_Color toggle_on;
    UK_Color toggle_off_bg;
    UK_Color toggle_on_bg;
    UK_Color toggle_knob;

    /* Checkbox */
    UK_Color checkbox_off;
    UK_Color checkbox_on;
    UK_Color checkbox_check;

    UK_Color check_bg;
    UK_Color check_on;
    UK_Color check_knob;

    /* Radio */
    UK_Color radio_off;
    UK_Color radio_on;
    UK_Color radio_dot;

    UK_Color radio_off_bg;
    UK_Color radio_on_bg;
    UK_Color radio_knob;

    /* Compatibility */
    UK_Color surface_hover;
    UK_Color surface_pressed;
    UK_Color control_border;

    /* Geometry */
    float corner;
    float control_corner;
    float spacing;

    float panel_radius;
    float button_radius;
    float control_radius;

} UK_Theme;

typedef struct {
    UK_WidgetState states[UK_STATE_MAX];
} UK_StatePool;

typedef struct {

    SDL_Renderer *ren;

    TTF_Font *font;
    TTF_Font *font_sm;
    TTF_Font *font_lg;
    TTF_Font *font_xs;

    UK_Theme th;
    UK_StatePool pool;

    /* Physical mouse coordinates */
    float raw_mx;
    float raw_my;

    /* Logical mouse coordinates */
    float mx;
    float my;

    bool mdown;
    bool mpress;
    bool mrelease;

    float dt;
    float time;

    int win_w;
    int win_h;

    /* Logical design dimensions */
    float design_w;
    float design_h;

    /* Actual scale */
    float scale;

    /* Letterbox offsets */
    float offset_x;
    float offset_y;

    /* Automatic layout */
    float root_x;
    float root_y;
    float root_w;

    float cx;
    float cy;
    float cw;

    /* Temporary glass/panel state */
    UK_Rect glass_rect;

    /* Accessibility / rendering */
    bool reduced_motion;
    bool high_contrast;

} UK;


/* ============================================================
 * Lifecycle
 * ============================================================ */

UK uk_make(SDL_Renderer *ren, TTF_Font *font);

void uk_begin(
    UK *uk,
    float mx,
    float my,
    bool md,
    bool mp,
    bool mr,
    float dt,
    int sw,
    int sh
);

void uk_end(UK *uk);

void uk_shutdown(UK *uk);


/* ============================================================
 * Theme
 * ============================================================ */

UK_Theme uk_default_theme(void);

void uk_set_theme(
    UK *uk,
    UK_Theme theme
);


/* ============================================================
 * Layout
 * ============================================================ */

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

void uk_spacing(
    UK *uk,
    float amount
);


/* ============================================================
 * Utility
 * ============================================================ */

UK_Color uk_rgba(
    float r,
    float g,
    float b,
    float a
);

UK_WidgetState *uk_state(
    UK *uk,
    unsigned id
);

float uk_scale(
    UK *uk,
    float v
);

bool uk_hit(
    UK *uk,
    UK_Rect r
);

float uk_clampf(
    float v,
    float lo,
    float hi
);

UK_Color uk_color_lerp(
    UK_Color a,
    UK_Color b,
    float t
);


/* ============================================================
 * Basic drawing
 * ============================================================ */

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
 * Background / materials
 * ============================================================ */

void uk_bg_render(
    UK *uk
);

void uk_bg_draw(
    UK *uk
);

void uk_glass_surface(
    UK *uk,
    UK_Rect r
);

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

void uk_glass_panel_begin(
    UK *uk,
    float x,
    float y,
    float w,
    float h
);

void uk_glass_panel_end(
    UK *uk
);


/* ============================================================
 * Text
 * ============================================================ */

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


/* ============================================================
 * Labels
 * ============================================================ */

float uk_heading(
    UK *uk,
    const char *text
);

float uk_label(
    UK *uk,
    const char *text
);

float uk_label_sm(
    UK *uk,
    const char *text
);

float uk_label_color(
    UK *uk,
    const char *text,
    UK_Color color
);


/* ============================================================
 * Buttons
 * ============================================================ */

bool uk_button(
    UK *uk,
    unsigned id,
    const char *text,
    float h
);

bool uk_button_accent(
    UK *uk,
    unsigned id,
    const char *text,
    float h
);

bool uk_button_ghost(
    UK *uk,
    unsigned id,
    const char *text,
    float h
);


/* ============================================================
 * Slider
 * ============================================================ */

float uk_slider(
    UK *uk,
    unsigned id,
    float value
);


/* ============================================================
 * Toggle
 * ============================================================ */

bool uk_toggle(
    UK *uk,
    unsigned id,
    const char *text,
    bool *value
);


/* ============================================================
 * Checkbox
 * ============================================================ */

bool uk_checkbox(
    UK *uk,
    unsigned id,
    const char *text,
    bool *value
);


/* ============================================================
 * Radio
 * ============================================================ */

bool uk_radio(
    UK *uk,
    unsigned id,
    const char *text,
    bool active
);


/* ============================================================
 * Inline helpers
 * ============================================================ */

static inline float uk_minf(
    float a,
    float b
) {
    return a < b ? a : b;
}

static inline float uk_maxf(
    float a,
    float b
) {
    return a > b ? a : b;
}

#ifdef __cplusplus
}
#endif

#endif

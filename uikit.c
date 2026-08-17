#include "uikit.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif


/* ============================================================
 * Internal helpers
 * ============================================================ */

static float uk_lerp(
    float a,
    float b,
    float t
) {
    return a + (b - a) * t;
}


static float uk_smooth(
    float current,
    float target,
    float speed,
    float dt
) {
    float k = 1.0f - expf(-speed * dt);

    if (k < 0.0f)
        k = 0.0f;

    if (k > 1.0f)
        k = 1.0f;

    return current + (target - current) * k;
}


static SDL_FColor uk_sdl_color(
    UK_Color c
) {
    SDL_FColor out;

    out.r = uk_clampf(c.r, 0.0f, 1.0f);
    out.g = uk_clampf(c.g, 0.0f, 1.0f);
    out.b = uk_clampf(c.b, 0.0f, 1.0f);
    out.a = uk_clampf(c.a, 0.0f, 1.0f);

    return out;
}


static SDL_FRect uk_screen_rect(
    UK *uk,
    UK_Rect r
) {
    SDL_FRect out;

    out.x = uk->offset_x + r.x * uk->scale;
    out.y = uk->offset_y + r.y * uk->scale;
    out.w = r.w * uk->scale;
    out.h = r.h * uk->scale;

    return out;
}


static float uk_screen_x(
    UK *uk,
    float x
) {
    return uk->offset_x + x * uk->scale;
}


static float uk_screen_y(
    UK *uk,
    float y
) {
    return uk->offset_y + y * uk->scale;
}


static void uk_set_color(
    UK *uk,
    UK_Color c
) {
    SDL_FColor sc = uk_sdl_color(c);

    SDL_SetRenderDrawColorFloat(
        uk->ren,
        sc.r,
        sc.g,
        sc.b,
        sc.a
    );
}


static void uk_blend(
    UK *uk
) {
    SDL_SetRenderDrawBlendMode(
        uk->ren,
        SDL_BLENDMODE_BLEND
    );
}


/* ============================================================
 * Math / colors
 * ============================================================ */

float uk_clampf(
    float v,
    float lo,
    float hi
) {
    if (v < lo)
        return lo;

    if (v > hi)
        return hi;

    return v;
}


UK_Color uk_rgba(
    float r,
    float g,
    float b,
    float a
) {
    UK_Color c;

    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;

    return c;
}


UK_Color uk_color_lerp(
    UK_Color a,
    UK_Color b,
    float t
) {
    t = uk_clampf(
        t,
        0.0f,
        1.0f
    );

    return (UK_Color){
        uk_lerp(a.r, b.r, t),
        uk_lerp(a.g, b.g, t),
        uk_lerp(a.b, b.b, t),
        uk_lerp(a.a, b.a, t)
    };
}


/* ============================================================
 * Theme
 * ============================================================ */

UK_Theme uk_default_theme(void)
{
    UK_Theme t;

    memset(
        &t,
        0,
        sizeof(t)
    );

    /*
     * macOS-inspired dark palette.
     *
     * Important:
     * foreground colors are deliberately bright enough that
     * translucent surfaces never produce white-on-white text.
     */

    t.background =
        uk_rgba(
            0.055f,
            0.058f,
            0.065f,
            1.0f
        );

    t.background_secondary =
        uk_rgba(
            0.075f,
            0.078f,
            0.088f,
            1.0f
        );

    t.content =
        uk_rgba(
            0.100f,
            0.105f,
            0.118f,
            1.0f
        );

    t.content_secondary =
        uk_rgba(
            0.125f,
            0.130f,
            0.145f,
            1.0f
        );


    /* Text */

    t.text =
        uk_rgba(
            0.965f,
            0.970f,
            0.985f,
            1.0f
        );

    t.text_secondary =
        uk_rgba(
            0.700f,
            0.715f,
            0.750f,
            1.0f
        );

    t.text_tertiary =
        uk_rgba(
            0.480f,
            0.495f,
            0.535f,
            1.0f
        );

    t.text_dim =
        t.text_secondary;

    t.text_bright =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );


    /* Apple-like system blue */

    t.accent =
        uk_rgba(
            0.200f,
            0.475f,
            1.000f,
            1.0f
        );

    t.accent2 =
        uk_rgba(
            0.330f,
            0.600f,
            1.000f,
            1.0f
        );

    t.accent_hover =
        uk_rgba(
            0.255f,
            0.525f,
            1.000f,
            1.0f
        );

    t.accent_pressed =
        uk_rgba(
            0.150f,
            0.390f,
            0.900f,
            1.0f
        );

    t.success =
        uk_rgba(
            0.260f,
            0.820f,
            0.460f,
            1.0f
        );

    t.danger =
        uk_rgba(
            1.000f,
            0.300f,
            0.300f,
            1.0f
        );

    t.warning =
        uk_rgba(
            1.000f,
            0.650f,
            0.180f,
            1.0f
        );


    /* Materials */

    t.material_thin =
        uk_rgba(
            0.15f,
            0.16f,
            0.18f,
            0.58f
        );

    t.material_regular =
        uk_rgba(
            0.12f,
            0.13f,
            0.15f,
            0.88f
        );

    t.material_thick =
        uk_rgba(
            0.09f,
            0.095f,
            0.11f,
            0.96f
        );


    /* Panels */

    t.panel =
        uk_rgba(
            0.115f,
            0.120f,
            0.135f,
            0.94f
        );

    t.panel_border =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            0.095f
        );

    t.panel_highlight =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            0.055f
        );


    /* Glass */

    t.glass_bg =
        uk_rgba(
            0.12f,
            0.13f,
            0.15f,
            0.88f
        );

    t.glass_border =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            0.10f
        );

    t.glass_highlight =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            0.045f
        );

    t.glass_shadow =
        uk_rgba(
            0.0f,
            0.0f,
            0.0f,
            0.35f
        );

    t.glass_tint =
        uk_rgba(
            0.20f,
            0.35f,
            0.60f,
            0.04f
        );


    /* Buttons */

    t.button =
        uk_rgba(
            0.18f,
            0.19f,
            0.215f,
            0.96f
        );

    t.button_hover =
        uk_rgba(
            0.23f,
            0.24f,
            0.27f,
            0.98f
        );

    t.button_pressed =
        uk_rgba(
            0.135f,
            0.145f,
            0.165f,
            1.0f
        );

    t.button_text =
        t.text;


    /* Old names */

    t.btn_bg =
        t.button;

    t.btn_hover =
        t.button_hover;

    t.btn_active =
        t.button_pressed;

    t.btn_text =
        t.button_text;


    /* Slider */

    t.slider_track =
        uk_rgba(
            0.22f,
            0.23f,
            0.26f,
            1.0f
        );

    t.slider_fill =
        t.accent;

    t.slider_knob =
        uk_rgba(
            0.96f,
            0.97f,
            1.0f,
            1.0f
        );


    /* Toggle */

    t.toggle_off =
        uk_rgba(
            0.30f,
            0.31f,
            0.34f,
            1.0f
        );

    t.toggle_on =
        t.accent;

    t.toggle_off_bg =
        t.toggle_off;

    t.toggle_on_bg =
        t.toggle_on;

    t.toggle_knob =
        uk_rgba(
            0.98f,
            0.985f,
            1.0f,
            1.0f
        );


    /* Checkbox */

    t.checkbox_off =
        uk_rgba(
            0.18f,
            0.19f,
            0.215f,
            1.0f
        );

    t.checkbox_on =
        t.accent;

    t.checkbox_check =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

    t.check_bg =
        t.checkbox_off;

    t.check_on =
        t.checkbox_on;

    t.check_knob =
        t.checkbox_check;


    /* Radio */

    t.radio_off =
        uk_rgba(
            0.38f,
            0.39f,
            0.42f,
            1.0f
        );

    t.radio_on =
        t.accent;

    t.radio_dot =
        uk_rgba(
            0.98f,
            0.99f,
            1.0f,
            1.0f
        );

    t.radio_off_bg =
        t.radio_off;

    t.radio_on_bg =
        t.radio_on;

    t.radio_knob =
        t.radio_dot;


    t.surface_hover =
        t.button_hover;

    t.surface_pressed =
        t.button_pressed;

    t.control_border =
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            0.09f
        );


    /* Geometry */

    t.corner = 12.0f;
    t.control_corner = 9.0f;

    t.spacing = 10.0f;

    t.panel_radius = 16.0f;
    t.button_radius = 9.0f;
    t.control_radius = 8.0f;

    return t;
}


void uk_set_theme(
    UK *uk,
    UK_Theme theme
) {
    if (!uk)
        return;

    uk->th = theme;
}


/* ============================================================
 * Lifecycle
 * ============================================================ */

UK uk_make(
    SDL_Renderer *ren,
    TTF_Font *font
) {
    UK uk;

    memset(
        &uk,
        0,
        sizeof(uk)
    );

    uk.ren = ren;

    uk.font = font;
    uk.font_sm = font;
    uk.font_lg = font;
    uk.font_xs = font;

    uk.th = uk_default_theme();

    uk.design_w = 1440.0f;
    uk.design_h = 900.0f;

    uk.scale = 1.0f;

    uk.root_x = 48.0f;
    uk.root_y = 48.0f;
    uk.root_w = 1344.0f;

    uk.cx = uk.root_x;
    uk.cy = uk.root_y;
    uk.cw = uk.root_w;

    uk.reduced_motion = false;
    uk.high_contrast = false;

    return uk;
}


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
) {
    if (!uk)
        return;

    uk->raw_mx = mx;
    uk->raw_my = my;

    uk->win_w = sw;
    uk->win_h = sh;

    uk->dt = dt;

    if (dt < 0.0f)
        dt = 0.0f;

    if (dt > 0.1f)
        dt = 0.1f;

    uk->time += dt;

    uk->mdown = md;
    uk->mpress = mp;
    uk->mrelease = mr;

    /*
     * Maintain a logical 1440x900 coordinate system while
     * preserving aspect ratio.
     */
    float sx =
        (float)sw / uk->design_w;

    float sy =
        (float)sh / uk->design_h;

    uk->scale =
        sx < sy ? sx : sy;

    if (uk->scale <= 0.0f)
        uk->scale = 1.0f;

    uk->offset_x =
        ((float)sw -
         uk->design_w * uk->scale) *
        0.5f;

    uk->offset_y =
        ((float)sh -
         uk->design_h * uk->scale) *
        0.5f;


    /* Convert physical mouse into logical coordinates. */

    uk->mx =
        (mx - uk->offset_x) /
        uk->scale;

    uk->my =
        (my - uk->offset_y) /
        uk->scale;


    /*
     * Keep mouse inside the logical canvas.
     */
    if (uk->mx < -1000.0f)
        uk->mx = -1000.0f;

    if (uk->my < -1000.0f)
        uk->my = -1000.0f;


    /* Reset automatic layout. */

    uk->root_x = 48.0f;
    uk->root_y = 48.0f;

    uk->root_w =
        uk->design_w - 96.0f;

    uk->cx = uk->root_x;
    uk->cy = uk->root_y;
    uk->cw = uk->root_w;


    uk_blend(uk);

    /*
     * Clear the entire physical renderer. This avoids stale
     * pixels when a window is resized.
     */
    uk_set_color(
        uk,
        uk->th.background
    );

    SDL_RenderClear(
        uk->ren
    );
}


void uk_end(
    UK *uk
) {
    (void)uk;
}


void uk_shutdown(
    UK *uk
) {
    if (!uk)
        return;
}


/* ============================================================
 * State
 * ============================================================ */

UK_WidgetState *uk_state(
    UK *uk,
    unsigned id
) {
    if (!uk)
        return NULL;

    id %= UK_STATE_MAX;

    return &uk->pool.states[id];
}


/* ============================================================
 * Layout
 * ============================================================ */

void uk_set_pos(
    UK *uk,
    float x,
    float y
) {
    if (!uk)
        return;

    uk->cx = x;
    uk->cy = y;
}


void uk_set_width(
    UK *uk,
    float w
) {
    if (!uk)
        return;

    uk->cw = w;
}


void uk_indent(
    UK *uk,
    float dx
) {
    if (!uk)
        return;

    uk->cx += dx;
    uk->cw -= dx;

    if (uk->cw < 0.0f)
        uk->cw = 0.0f;
}


void uk_spacing(
    UK *uk,
    float amount
) {
    if (!uk)
        return;

    uk->cy += amount;
}


float uk_scale(
    UK *uk,
    float v
) {
    if (!uk)
        return v;

    return v * uk->scale;
}


bool uk_hit(
    UK *uk,
    UK_Rect r
) {
    if (!uk)
        return false;

    return
        uk->mx >= r.x &&
        uk->mx <= r.x + r.w &&
        uk->my >= r.y &&
        uk->my <= r.y + r.h;
}


/* ============================================================
 * Basic geometry
 * ============================================================ */

void uk_rect_fill(
    UK *uk,
    UK_Rect r,
    UK_Color c
) {
    if (!uk || !uk->ren)
        return;

    SDL_FRect sr =
        uk_screen_rect(
            uk,
            r
        );

    uk_set_color(
        uk,
        c
    );

    SDL_RenderFillRect(
        uk->ren,
        &sr
    );
}


/*
 * Rounded rectangle using clean horizontal strips.
 *
 * This avoids textured blur and avoids the blurry edges produced
 * by scaling low-resolution masks.
 */
void uk_rect_fill_r(
    UK *uk,
    UK_Rect r,
    float radius,
    UK_Color c
) {
    if (!uk || !uk->ren)
        return;

    if (radius <= 0.5f) {
        uk_rect_fill(
            uk,
            r,
            c
        );
        return;
    }

    radius =
        uk_minf(
            radius,
            uk_minf(
                r.w * 0.5f,
                r.h * 0.5f
            )
        );

    float x0 = r.x;
    float x1 = r.x + r.w;

    float y0 = r.y;
    float y1 = r.y + r.h;

    /*
     * Center.
     */
    uk_rect_fill(
        uk,
        (UK_Rect){
            x0 + radius,
            y0,
            r.w - radius * 2.0f,
            r.h
        },
        c
    );

    /*
     * Left and right body.
     */
    uk_rect_fill(
        uk,
        (UK_Rect){
            x0,
            y0 + radius,
            radius,
            r.h - radius * 2.0f
        },
        c
    );

    uk_rect_fill(
        uk,
        (UK_Rect){
            x1 - radius,
            y0 + radius,
            radius,
            r.h - radius * 2.0f
        },
        c
    );

    /*
     * Rounded corners.
     */
    int steps =
        (int)(radius * uk->scale);

    if (steps < 8)
        steps = 8;

    if (steps > 48)
        steps = 48;

    float rs = radius * uk->scale;

    for (int i = 0; i < steps; ++i) {

        float a0 =
            (float)i /
            (float)steps;

        float a1 =
            (float)(i + 1) /
            (float)steps;

        float top =
            a0 * (float)(M_PI * 0.5);

        float bot =
            a1 * (float)(M_PI * 0.5);

        float sx0 =
            cosf(top) * rs;

        float sx1 =
            cosf(bot) * rs;

        float dx =
            fabsf(sx0 - sx1);

        float yoff =
            sinf(
                (top + bot) * 0.5f
            ) * rs;

        float logical_h =
            (dx / uk->scale) + 1.0f;

        float logical_y =
            y0 + radius -
            yoff / uk->scale;

        uk_rect_fill(
            uk,
            (UK_Rect){
                x0 + radius -
                    dx / uk->scale,
                logical_y,
                dx / uk->scale +
                    1.0f,
                logical_h
            },
            c
        );

        uk_rect_fill(
            uk,
            (UK_Rect){
                x1 - radius,
                logical_y,
                dx / uk->scale +
                    1.0f,
                logical_h
            },
            c
        );

        float bottom_y =
            y1 - radius +
            yoff / uk->scale;

        uk_rect_fill(
            uk,
            (UK_Rect){
                x0 + radius -
                    dx / uk->scale,
                bottom_y,
                dx / uk->scale +
                    1.0f,
                logical_h
            },
            c
        );

        uk_rect_fill(
            uk,
            (UK_Rect){
                x1 - radius,
                bottom_y,
                dx / uk->scale +
                    1.0f,
                logical_h
            },
            c
        );
    }
}


void uk_rect_stroke_r(
    UK *uk,
    UK_Rect r,
    float radius,
    UK_Color c,
    float thickness
) {
    if (!uk)
        return;

    if (thickness <= 0.0f)
        return;

    /*
     * Four clean strips.
     */
    uk_rect_fill_r(
        uk,
        (UK_Rect){
            r.x,
            r.y,
            r.w,
            thickness
        },
        radius,
        c
    );

    uk_rect_fill_r(
        uk,
        (UK_Rect){
            r.x,
            r.y + r.h - thickness,
            r.w,
            thickness
        },
        radius,
        c
    );

    uk_rect_fill(
        uk,
        (UK_Rect){
            r.x,
            r.y + radius,
            thickness,
            r.h - radius * 2.0f
        },
        c
    );

    uk_rect_fill(
        uk,
        (UK_Rect){
            r.x + r.w - thickness,
            r.y + radius,
            thickness,
            r.h - radius * 2.0f
        },
        c
    );
}


void uk_circle(
    UK *uk,
    float cx,
    float cy,
    float radius,
    UK_Color c
) {
    if (!uk || !uk->ren)
        return;

    float sr =
        radius * uk->scale;

    if (sr <= 0.5f)
        return;

    int segments =
        (int)(sr * 0.8f);

    if (segments < 24)
        segments = 24;

    if (segments > 96)
        segments = 96;

    /*
     * Use SDL geometry with float colors.
     */
    int count =
        segments + 2;

    SDL_Vertex *v =
        (SDL_Vertex *)calloc(
            (size_t)count,
            sizeof(SDL_Vertex)
        );

    if (!v)
        return;

    SDL_FColor sc =
        uk_sdl_color(c);

    float sx =
        uk_screen_x(
            uk,
            cx
        );

    float sy =
        uk_screen_y(
            uk,
            cy
        );

    v[0].position.x = sx;
    v[0].position.y = sy;
    v[0].color = sc;

    for (int i = 0; i <= segments; ++i) {

        float a =
            (float)i /
            (float)segments *
            (float)(M_PI * 2.0);

        v[i + 1].position.x =
            sx + cosf(a) * sr;

        v[i + 1].position.y =
            sy + sinf(a) * sr;

        v[i + 1].color = sc;
    }

    int *indices =
        (int *)malloc(
            sizeof(int) *
            (size_t)segments *
            3
        );

    if (!indices) {
        free(v);
        return;
    }

    for (int i = 0; i < segments; ++i) {
        indices[i * 3 + 0] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = i + 2;
    }

    SDL_RenderGeometry(
        uk->ren,
        NULL,
        v,
        count,
        indices,
        segments * 3
    );

    free(indices);
    free(v);
}


void uk_line(
    UK *uk,
    float x0,
    float y0,
    float x1,
    float y1,
    UK_Color c,
    float thickness
) {
    if (!uk)
        return;

    /*
     * SDL's line width isn't consistently available across
     * render backends, so draw a rotated quad.
     */
    float dx = x1 - x0;
    float dy = y1 - y0;

    float len =
        sqrtf(
            dx * dx +
            dy * dy
        );

    if (len < 0.001f)
        return;

    float nx =
        -dy / len *
        thickness *
        0.5f;

    float ny =
        dx / len *
        thickness *
        0.5f;

    SDL_Vertex v[4];

    SDL_FColor sc =
        uk_sdl_color(c);

    float ax =
        uk_screen_x(
            uk,
            x0 + nx
        );

    float ay =
        uk_screen_y(
            uk,
            y0 + ny
        );

    float bx =
        uk_screen_x(
            uk,
            x1 + nx
        );

    float by =
        uk_screen_y(
            uk,
            y1 + ny
        );

    float cx =
        uk_screen_x(
            uk,
            x1 - nx
        );

    float cy =
        uk_screen_y(
            uk,
            y1 - ny
        );

    float dx2 =
        uk_screen_x(
            uk,
            x0 - nx
        );

    float dy2 =
        uk_screen_y(
            uk,
            y0 - ny
        );

    v[0].position.x = ax;
    v[0].position.y = ay;

    v[1].position.x = bx;
    v[1].position.y = by;

    v[2].position.x = cx;
    v[2].position.y = cy;

    v[3].position.x = dx2;
    v[3].position.y = dy2;

    for (int i = 0; i < 4; ++i)
        v[i].color = sc;

    int indices[6] = {
        0, 1, 2,
        0, 2, 3
    };

    SDL_RenderGeometry(
        uk->ren,
        NULL,
        v,
        4,
        indices,
        6
    );
}


/* ============================================================
 * Background
 * ============================================================ */

void uk_bg_render(
    UK *uk
) {
    if (!uk)
        return;

    /*
     * Very subtle vertical depth.
     *
     * No giant glowing orbs.
     */
    const int strips = 90;

    float h =
        uk->design_h /
        (float)strips;

    for (int i = 0; i < strips; ++i) {

        float t =
            (float)i /
            (float)(strips - 1);

        UK_Color c =
            uk_color_lerp(
                uk->th.background,
                uk->th.background_secondary,
                t * 0.30f
            );

        uk_rect_fill(
            uk,
            (UK_Rect){
                0.0f,
                i * h,
                uk->design_w,
                h + 1.0f
            },
            c
        );
    }
}


void uk_bg_draw(
    UK *uk
) {
    uk_bg_render(uk);
}


/* ============================================================
 * Materials
 * ============================================================ */

void uk_glass_surface(
    UK *uk,
    UK_Rect r
) {
    if (!uk)
        return;

    /*
     * Shadow.
     */
    uk_rect_fill_r(
        uk,
        (UK_Rect){
            r.x,
            r.y + 3.0f,
            r.w,
            r.h + 4.0f
        },
        uk->th.panel_radius,
        uk->th.glass_shadow
    );

    /*
     * Material.
     */
    uk_rect_fill_r(
        uk,
        r,
        uk->th.panel_radius,
        uk->th.glass_bg
    );

    /*
     * Very subtle tint.
     */
    uk_rect_fill_r(
        uk,
        r,
        uk->th.panel_radius,
        uk->th.glass_tint
    );

    /*
     * Top highlight.
     */
    uk_rect_fill_r(
        uk,
        (UK_Rect){
            r.x + 1.0f,
            r.y + 1.0f,
            r.w - 2.0f,
            1.0f
        },
        1.0f,
        uk->th.glass_highlight
    );

    /*
     * Border.
     */
    uk_rect_stroke_r(
        uk,
        r,
        uk->th.panel_radius,
        uk->th.glass_border,
        1.0f
    );
}


void uk_glass_begin(
    UK *uk,
    float x,
    float y,
    float w,
    float h
) {
    if (!uk)
        return;

    uk->glass_rect =
        (UK_Rect){
            x,
            y,
            w,
            h
        };

    uk_glass_surface(
        uk,
        uk->glass_rect
    );

    uk->cx =
        x + 24.0f;

    uk->cy =
        y + 24.0f;

    uk->cw =
        w - 48.0f;
}


void uk_glass_end(
    UK *uk,
    float x,
    float y,
    float w,
    float h
) {
    (void)x;
    (void)y;
    (void)w;
    (void)h;

    if (!uk)
        return;

    uk->cx =
        uk->root_x;

    uk->cy =
        uk->root_y;
}


void uk_glass_panel_begin(
    UK *uk,
    float x,
    float y,
    float w,
    float h
) {
    uk_glass_begin(
        uk,
        x,
        y,
        w,
        h
    );
}


void uk_glass_panel_end(
    UK *uk
) {
    if (!uk)
        return;

    uk->cx =
        uk->root_x;

    uk->cy =
        uk->root_y;
}


/* ============================================================
 * Text
 * ============================================================ */

float uk_text_w(
    TTF_Font *font,
    const char *text
) {
    if (!font || !text)
        return 0.0f;

    int w = 0;

    int h = 0;

    if (!TTF_GetStringSize(
            font,
            text,
            0,
            &w,
            &h
        )) {
        return 0.0f;
    }

    return (float)w;
}


int uk_text_h(
    TTF_Font *font
) {
    if (!font)
        return 0;

    return TTF_GetFontHeight(font);
}


void uk_text_font(
    UK *uk,
    TTF_Font *font,
    const char *text,
    float x,
    float y,
    UK_Color color
) {
    if (!uk ||
        !uk->ren ||
        !font ||
        !text ||
        !text[0])
        return;

    SDL_Color c;

    c.r =
        (Uint8)(
            uk_clampf(
                color.r,
                0.0f,
                1.0f
            ) * 255.0f
        );

    c.g =
        (Uint8)(
            uk_clampf(
                color.g,
                0.0f,
                1.0f
            ) * 255.0f
        );

    c.b =
        (Uint8)(
            uk_clampf(
                color.b,
                0.0f,
                1.0f
            ) * 255.0f
        );

    c.a =
        (Uint8)(
            uk_clampf(
                color.a,
                0.0f,
                1.0f
            ) * 255.0f
        );

    SDL_Surface *surface =
        TTF_RenderText_Blended(
            font,
            text,
            0,
            c
        );

    if (!surface)
        return;

    SDL_Texture *texture =
        SDL_CreateTextureFromSurface(
            uk->ren,
            surface
        );

    if (!texture) {
        SDL_DestroySurface(surface);
        return;
    }

    SDL_SetTextureBlendMode(
        texture,
        SDL_BLENDMODE_BLEND
    );

    SDL_FRect dst;

    dst.x =
        uk_screen_x(
            uk,
            x
        );

    dst.y =
        uk_screen_y(
            uk,
            y
        );

    dst.w =
        surface->w *
        uk->scale;

    dst.h =
        surface->h *
        uk->scale;

    SDL_RenderTexture(
        uk->ren,
        texture,
        NULL,
        &dst
    );

    SDL_DestroyTexture(texture);
    SDL_DestroySurface(surface);
}


void uk_text(
    UK *uk,
    const char *text,
    float x,
    float y,
    UK_Color color
) {
    if (!uk)
        return;

    uk_text_font(
        uk,
        uk->font,
        text,
        x,
        y,
        color
    );
}


/* ============================================================
 * Labels
 * ============================================================ */

float uk_heading(
    UK *uk,
    const char *text
) {
    if (!uk)
        return 0.0f;

    TTF_Font *font =
        uk->font_lg ?
        uk->font_lg :
        uk->font;

    uk_text_font(
        uk,
        font,
        text,
        uk->cx,
        uk->cy,
        uk->th.text_bright
    );

    float h =
        (float)uk_text_h(font);

    uk->cy += h + 14.0f;

    return h;
}


float uk_label(
    UK *uk,
    const char *text
) {
    if (!uk)
        return 0.0f;

    TTF_Font *font =
        uk->font ?
        uk->font :
        uk->font_sm;

    uk_text_font(
        uk,
        font,
        text,
        uk->cx,
        uk->cy,
        uk->th.text
    );

    float h =
        (float)uk_text_h(font);

    uk->cy += h + 8.0f;

    return h;
}


float uk_label_sm(
    UK *uk,
    const char *text
) {
    if (!uk)
        return 0.0f;

    TTF_Font *font =
        uk->font_sm ?
        uk->font_sm :
        uk->font;

    uk_text_font(
        uk,
        font,
        text,
        uk->cx,
        uk->cy,
        uk->th.text_secondary
    );

    float h =
        (float)uk_text_h(font);

    uk->cy += h + 7.0f;

    return h;
}


float uk_label_color(
    UK *uk,
    const char *text,
    UK_Color color
) {
    if (!uk)
        return 0.0f;

    TTF_Font *font =
        uk->font_sm ?
        uk->font_sm :
        uk->font;

    uk_text_font(
        uk,
        font,
        text,
        uk->cx,
        uk->cy,
        color
    );

    float h =
        (float)uk_text_h(font);

    uk->cy += h + 7.0f;

    return h;
}


/* ============================================================
 * Button internals
 * ============================================================ */

static bool uk_button_impl(
    UK *uk,
    unsigned id,
    const char *text,
    float h,
    UK_Color normal,
    UK_Color hover,
    UK_Color pressed,
    UK_Color text_color,
    bool accent
) {
    if (!uk)
        return false;

    if (h < 32.0f)
        h = 32.0f;

    if (h > 64.0f)
        h = 64.0f;

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        h
    };

    UK_WidgetState *s =
        uk_state(
            uk,
            id
        );

    bool hot =
        uk_hit(
            uk,
            r
        );

    bool click =
        hot &&
        uk->mpress;

    bool down =
        hot &&
        uk->mdown;

    if (!s->initialized) {
        s->anim = hot ? 1.0f : 0.0f;
        s->initialized = true;
    }

    s->hovered = hot;
    s->pressed = down;

    float target =
        hot ? 1.0f : 0.0f;

    if (uk->reduced_motion)
        s->anim = target;
    else
        s->anim =
            uk_smooth(
                s->anim,
                target,
                14.0f,
                uk->dt
            );

    UK_Color fill =
        uk_color_lerp(
            normal,
            hover,
            s->anim
        );

    if (down)
        fill =
            pressed;

    /*
     * Small physical shadow.
     */
    if (!accent) {
        uk_rect_fill_r(
            uk,
            (UK_Rect){
                r.x,
                r.y + 2.0f,
                r.w,
                r.h
            },
            uk->th.button_radius,
            uk->th.glass_shadow
        );
    }

    uk_rect_fill_r(
        uk,
        r,
        uk->th.button_radius,
        fill
    );

    /*
     * Very subtle top highlight.
     */
    if (!down) {
        uk_rect_fill_r(
            uk,
            (UK_Rect){
                r.x + 1.0f,
                r.y + 1.0f,
                r.w - 2.0f,
                1.0f
            },
            1.0f,
            uk->th.panel_highlight
        );
    }

    uk_rect_stroke_r(
        uk,
        r,
        uk->th.button_radius,
        uk->th.control_border,
        1.0f
    );

    TTF_Font *font =
        uk->font_sm ?
        uk->font_sm :
        uk->font;

    float tw =
        uk_text_w(
            font,
            text
        );

    float th =
        (float)uk_text_h(font);

    float tx =
        r.x +
        (r.w - tw) * 0.5f;

    float ty =
        r.y +
        (r.h - th) * 0.5f;

    uk_text_font(
        uk,
        font,
        text,
        tx,
        ty,
        text_color
    );

    uk->cy +=
        h + 9.0f;

    return click;
}


/* ============================================================
 * Buttons
 * ============================================================ */

bool uk_button(
    UK *uk,
    unsigned id,
    const char *text,
    float h
) {
    if (!uk)
        return false;

    return uk_button_impl(
        uk,
        id,
        text,
        h,
        uk->th.button,
        uk->th.button_hover,
        uk->th.button_pressed,
        uk->th.button_text,
        false
    );
}


bool uk_button_accent(
    UK *uk,
    unsigned id,
    const char *text,
    float h
) {
    if (!uk)
        return false;

    return uk_button_impl(
        uk,
        id,
        text,
        h,
        uk->th.accent,
        uk->th.accent_hover,
        uk->th.accent_pressed,
        uk_rgba(
            1.0f,
            1.0f,
            1.0f,
            1.0f
        ),
        true
    );
}


bool uk_button_ghost(
    UK *uk,
    unsigned id,
    const char *text,
    float h
) {
    if (!uk)
        return false;

    return uk_button_impl(
        uk,
        id,
        text,
        h,
        uk_rgba(
            0.0f,
            0.0f,
            0.0f,
            0.0f
        ),
        uk->th.surface_hover,
        uk->th.surface_pressed,
        uk->th.text,
        false
    );
}


/* ============================================================
 * Slider
 * ============================================================ */

float uk_slider(
    UK *uk,
    unsigned id,
    float value
) {
    if (!uk)
        return value;

    value =
        uk_clampf(
            value,
            0.0f,
            1.0f
        );

    UK_WidgetState *s =
        uk_state(
            uk,
            id
        );

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        28.0f
    };

    float track_y =
        r.y + 12.0f;

    float track_h =
        5.0f;

    UK_Rect track = {
        r.x,
        track_y,
        r.w,
        track_h
    };

    bool hot =
        uk_hit(
            uk,
            r
        );

    if (!s->initialized) {
        s->value = value;
        s->initialized = true;
    }

    if (hot && uk->mpress)
        s->dragging = true;

    if (!uk->mdown)
        s->dragging = false;

    if (s->dragging) {

        float t =
            (uk->mx - r.x) /
            r.w;

        s->value =
            uk_clampf(
                t,
                0.0f,
                1.0f
            );
    } else {
        s->value =
            uk_smooth(
                s->value,
                value,
                16.0f,
                uk->dt
            );
    }

    value =
        s->value;

    uk_rect_fill_r(
        uk,
        track,
        3.0f,
        uk->th.slider_track
    );

    uk_rect_fill_r(
        uk,
        (UK_Rect){
            r.x,
            track_y,
            r.w * value,
            track_h
        },
        3.0f,
        uk->th.slider_fill
    );

    float knob_x =
        r.x +
        r.w * value;

    uk_circle(
        uk,
        knob_x,
        track_y + track_h * 0.5f,
        8.0f,
        uk->th.slider_knob
    );

    /*
     * Slight accent ring when active.
     */
    if (s->dragging) {
        uk_circle(
            uk,
            knob_x,
            track_y + track_h * 0.5f,
            10.0f,
            uk_rgba(
                uk->th.accent.r,
                uk->th.accent.g,
                uk->th.accent.b,
                0.20f
            )
        );

        uk_circle(
            uk,
            knob_x,
            track_y + track_h * 0.5f,
            7.0f,
            uk->th.slider_knob
        );
    }

    uk->cy += 38.0f;

    return value;
}


/* ============================================================
 * Toggle
 * ============================================================ */

bool uk_toggle(
    UK *uk,
    unsigned id,
    const char *text,
    bool *value
) {
    if (!uk || !value)
        return false;

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        30.0f
    };

    UK_Rect sw = {
        r.x + r.w - 50.0f,
        r.y + 2.0f,
        50.0f,
        26.0f
    };

    bool hot =
        uk_hit(
            uk,
            r
        );

    bool changed = false;

    if (hot && uk->mpress) {
        *value = !*value;
        changed = true;
    }

    UK_WidgetState *s =
        uk_state(
            uk,
            id
        );

    float target =
        *value ? 1.0f : 0.0f;

    if (!s->initialized) {
        s->anim = target;
        s->initialized = true;
    }

    if (uk->reduced_motion)
        s->anim = target;
    else
        s->anim =
            uk_smooth(
                s->anim,
                target,
                18.0f,
                uk->dt
            );

    UK_Color bg =
        uk_color_lerp(
            uk->th.toggle_off,
            uk->th.toggle_on,
            s->anim
        );

    uk_rect_fill_r(
        uk,
        sw,
        13.0f,
        bg
    );

    float knob_x =
        sw.x +
        13.0f +
        s->anim *
        (sw.w - 26.0f);

    uk_circle(
        uk,
        knob_x,
        sw.y + 13.0f,
        10.0f,
        uk->th.toggle_knob
    );

    if (text && text[0]) {
        TTF_Font *font =
            uk->font_sm ?
            uk->font_sm :
            uk->font;

        float th =
            (float)uk_text_h(font);

        uk_text_font(
            uk,
            font,
            text,
            r.x,
            r.y +
                (r.h - th) * 0.5f,
            uk->th.text
        );
    }

    uk->cy += 38.0f;

    return changed;
}


/* ============================================================
 * Checkbox
 * ============================================================ */

bool uk_checkbox(
    UK *uk,
    unsigned id,
    const char *text,
    bool *value
) {
    if (!uk || !value)
        return false;

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        30.0f
    };

    UK_Rect box = {
        r.x,
        r.y + 2.0f,
        22.0f,
        22.0f
    };

    bool hot =
        uk_hit(
            uk,
            r
        );

    bool changed = false;

    if (hot && uk->mpress) {
        *value = !*value;
        changed = true;
    }

    UK_Color bg =
        *value ?
        uk->th.checkbox_on :
        uk->th.checkbox_off;

    uk_rect_fill_r(
        uk,
        box,
        6.0f,
        bg
    );

    if (*value) {

        /*
         * Clean check mark.
         */
        uk_line(
            uk,
            box.x + 5.0f,
            box.y + 11.0f,
            box.x + 9.0f,
            box.y + 15.0f,
            uk->th.checkbox_check,
            2.0f
        );

        uk_line(
            uk,
            box.x + 9.0f,
            box.y + 15.0f,
            box.x + 17.0f,
            box.y + 7.0f,
            uk->th.checkbox_check,
            2.0f
        );
    } else {
        uk_rect_stroke_r(
            uk,
            box,
            6.0f,
            uk->th.control_border,
            1.0f
        );
    }

    if (text && text[0]) {

        TTF_Font *font =
            uk->font_sm ?
            uk->font_sm :
            uk->font;

        float th =
            (float)uk_text_h(font);

        uk_text_font(
            uk,
            font,
            text,
            box.x + 34.0f,
            r.y +
                (r.h - th) * 0.5f,
            uk->th.text
        );
    }

    uk->cy += 38.0f;

    (void)id;

    return changed;
}


/* ============================================================
 * Radio
 * ============================================================ */

bool uk_radio(
    UK *uk,
    unsigned id,
    const char *text,
    bool active
) {
    if (!uk)
        return false;

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        30.0f
    };

    bool hot =
        uk_hit(
            uk,
            r
        );

    bool clicked =
        hot &&
        uk->mpress;

    float cx =
        r.x + 11.0f;

    float cy =
        r.y + 15.0f;

    uk_circle(
        uk,
        cx,
        cy,
        10.5f,
        active ?
            uk->th.radio_on :
            uk->th.radio_off
    );

    if (active) {
        uk_circle(
            uk,
            cx,
            cy,
            5.0f,
            uk->th.radio_dot
        );
    } else {
        uk_circle(
            uk,
            cx,
            cy,
            7.5f,
            uk->th.background_secondary
        );
    }

    if (text && text[0]) {

        TTF_Font *font =
            uk->font_sm ?
            uk->font_sm :
            uk->font;

        float th =
            (float)uk_text_h(font);

        uk_text_font(
            uk,
            font,
            text,
            r.x + 30.0f,
            r.y +
                (r.h - th) * 0.5f,
            uk->th.text
        );
    }

    uk->cy += 38.0f;

    (void)id;

    return clicked;
}

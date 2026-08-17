#include "uikit.h"
#include <string.h>
#include <math.h>

static UK_Color button_mix(
    UK_Color a,
    UK_Color b,
    float t
) {
    return uk_color_lerp(a, b, t);
}

static bool button_impl(
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
    if (!uk || !text) return false;

    if (h <= 0.0f)
        h = 40.0f;

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        h
    };

    bool hot = uk_hit(uk, r);
    bool clicked = false;

    if (hot && uk->mpress)
        clicked = true;

    UK_WidgetState *st = uk_state(uk, id);

    if (!st->initialized) {
        st->initialized = true;
        st->anim = hot ? 1.0f : 0.0f;
    }

    float target = hot ? 1.0f : 0.0f;

    /*
     * Smooth hover animation.
     * This is deliberately subtle; macOS controls don't
     * snap between huge visual states.
     */
    float speed = 12.0f;
    st->anim += (target - st->anim) *
                uk_clampf(uk->dt * speed, 0.0f, 1.0f);

    UK_Color bg = button_mix(
        normal,
        hover,
        st->anim
    );

    if (uk->mdown && hot) {
        bg = button_mix(
            bg,
            pressed,
            0.72f
        );
    }

    float radius = 10.0f;

    /*
     * Very soft shadow underneath.
     */
    UK_Rect shadow = {
        r.x,
        r.y + 1.0f,
        r.w,
        r.h
    };

    UK_Color shadow_color = {
        0.0f,
        0.0f,
        0.0f,
        accent ? 0.16f : 0.12f
    };

    uk_rect_fill_r(
        uk,
        shadow,
        radius,
        shadow_color
    );

    /*
     * Main button surface.
     */
    uk_rect_fill_r(
        uk,
        r,
        radius,
        bg
    );

    /*
     * Thin, low contrast border.
     */
    UK_Color border;

    if (accent) {
        border = (UK_Color){
            1.0f,
            1.0f,
            1.0f,
            0.18f
        };
    } else {
        border = uk->th.glass_border;

        if (border.a <= 0.0f) {
            border = (UK_Color){
                1.0f,
                1.0f,
                1.0f,
                0.12f
            };
        }
    }

    uk_rect_stroke_r(
        uk,
        r,
        radius,
        border,
        1.0f
    );

    /*
     * Tiny top highlight. This gives the surface
     * depth without making it look glossy.
     */
    if (!accent) {
        UK_Rect highlight = {
            r.x + 1.0f,
            r.y + 1.0f,
            r.w - 2.0f,
            1.0f
        };

        uk_rect_fill(
            uk,
            highlight,
            (UK_Color){
                1.0f,
                1.0f,
                1.0f,
                0.055f + st->anim * 0.025f
            }
        );
    }

    /*
     * Center text.
     */
    float tw = uk_text_w(uk->font, text);
    int th = uk_text_h(uk->font);

    float tx = r.x + (r.w - tw) * 0.5f;
    float ty = r.y + (r.h - (float)th) * 0.5f;

    if (uk->mdown && hot)
        ty += 0.5f;

    uk_text(
        uk,
        text,
        tx,
        ty,
        text_color
    );

    uk->cy += h + uk->th.spacing;

    return clicked;
}

bool uk_button(
    UK *uk,
    unsigned id,
    const char *text,
    float h
) {
    return button_impl(
        uk,
        id,
        text,
        h,
        uk->th.btn_bg,
        uk->th.btn_hover,
        uk->th.btn_active,
        uk->th.btn_text,
        false
    );
}

bool uk_button_accent(
    UK *uk,
    unsigned id,
    const char *text,
    float h
) {
    UK_Color normal = uk->th.accent;

    UK_Color hover = uk->th.accent;

    hover.r = uk_clampf(hover.r + 0.045f, 0.0f, 1.0f);
    hover.g = uk_clampf(hover.g + 0.045f, 0.0f, 1.0f);
    hover.b = uk_clampf(hover.b + 0.045f, 0.0f, 1.0f);

    UK_Color pressed = uk->th.accent;

    pressed.r *= 0.78f;
    pressed.g *= 0.78f;
    pressed.b *= 0.78f;

    UK_Color text_color = {
        1.0f,
        1.0f,
        1.0f,
        1.0f
    };

    return button_impl(
        uk,
        id,
        text,
        h,
        normal,
        hover,
        pressed,
        text_color,
        true
    );
}

bool uk_button_ghost(
    UK *uk,
    unsigned id,
    const char *text,
    float h
) {
    UK_Color normal = {
        1.0f,
        1.0f,
        1.0f,
        0.035f
    };

    UK_Color hover = {
        1.0f,
        1.0f,
        1.0f,
        0.085f
    };

    UK_Color pressed = {
        1.0f,
        1.0f,
        1.0f,
        0.12f
    };

    UK_Color text_color = uk->th.text;

    return button_impl(
        uk,
        id,
        text,
        h,
        normal,
        hover,
        pressed,
        text_color,
        false
    );
}

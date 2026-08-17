#ifndef UK_TOGGLE_H
#define UK_TOGGLE_H

#include "uikit.h"

static inline bool uk_toggle(UK *uk, bool *val, const char *label) {
    float s = uk->scale;
    float x = uk->cx, y = uk->cy;
    float w = uk->cw;
    UK_Theme *th = &uk->th;
    float tw = 44 * s, th_ = 26 * s;

    /* full row clickable */
    bool hover = uk->mx >= x && uk->mx <= x+w &&
                 uk->my >= y && uk->my <= y + th_ + 8*s;
    bool clicked = hover && uk->mpress;
    if (clicked) *val = !*val;

    /* animate */
    UK_WidgetState *st = uk_state(uk, (unsigned)(uintptr_t)val);
    float target = *val ? 1.0f : 0.0f;
    st->anim += (target - st->anim) * uk->dt * 14;
    if (fabsf(st->anim - target) < 0.005f) st->anim = target;

    /* label */
    if (label) {
        uk_text(uk, label, x, y + (th_ - (float)uk_text_h(uk->font)*s) * 0.5f,
                th->text);
    }

    /* track */
    float hit_x = x + w - tw;
    UK_Color track_c = uk_color_lerp(th->toggle_off_bg, th->toggle_on_bg, st->anim);
    uk_rect_fill_r(uk, (UK_Rect){ hit_x, y, tw, th_ }, th_ * 0.5f, track_c);

    /* inner highlight */
    uk_rect_fill_r(uk, (UK_Rect){ hit_x, y, tw, th_ * 0.5f }, th_ * 0.5f,
                    (UK_Color){ 255, 255, 255, (Uint8)(st->anim * 20 + 8) });

    /* knob */
    float kr = th_ * 0.5f - 3 * s;
    float kx = hit_x + th_ * 0.5f + st->anim * (tw - th_);
    float ky = y + th_ * 0.5f;

    uk_circle(uk, kx, ky + 1*s, kr + 1*s, (UK_Color){ 0, 0, 0, 30 });
    uk_circle(uk, kx, ky, kr, th->toggle_knob);
    /* specular */
    uk_circle(uk, kx - kr*0.2f, ky - kr*0.3f, kr*0.3f,
              (UK_Color){ 255, 255, 255, 100 });

    uk->cy += th_ + 8 * s;
    return clicked;
}

#endif

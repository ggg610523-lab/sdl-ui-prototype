#ifndef UK_CHECKBOX_H
#define UK_CHECKBOX_H

#include "uikit.h"

static inline bool uk_checkbox(UK *uk, bool *val, const char *label) {
    float s = uk->scale;
    float x = uk->cx, y = uk->cy;
    UK_Theme *th = &uk->th;
    float sz = 22 * s;
    float row_h = sz + 8 * s;

    /* full row clickable */
    bool hover = uk->mx >= x && uk->mx <= x + uk->cw &&
                 uk->my >= y && uk->my <= y + row_h;
    bool clicked = hover && uk->mpress;
    if (clicked) *val = !*val;

    /* animate */
    UK_WidgetState *st = uk_state(uk, (unsigned)(uintptr_t)val + 1000);
    float target = *val ? 1.0f : 0.0f;
    st->anim += (target - st->anim) * uk->dt * 14;
    if (fabsf(st->anim - target) < 0.005f) st->anim = target;

    /* box */
    UK_Color bg = uk_color_lerp(th->check_bg, th->check_on, st->anim);
    float rad = sz * 0.28f;
    uk_rect_fill_r(uk, (UK_Rect){x, y, sz, sz}, rad, bg);
    uk_rect_stroke_r(uk, (UK_Rect){x, y, sz, sz}, rad,
                      (UK_Color){ 255, 255, 255, (Uint8)(20 + st->anim * 25) }, 1);

    /* checkmark */
    if (st->anim > 0.01f) {
        float cx_ = x + sz * 0.5f;
        float cy_ = y + sz * 0.48f;
        float sc = sz * 0.28f;
        float a = st->anim;
        UK_Color kc = th->check_knob;
        kc.a = (Uint8)(a * 255);

        float x1l = cx_ - sc, y1l = cy_ + sc * 0.1f;
        float x2l = cx_ - sc * 0.1f, y2l = cy_ + sc * 0.8f;
        uk_line(uk, x1l, y1l,
                x1l + (x2l-x1l)*a, y1l + (y2l-y1l)*a, kc, 2.5f*s);

        if (a > 0.5f) {
            float a2 = (a - 0.5f) * 2.0f;
            float x1r = cx_ - sc*0.1f, y1r = cy_ + sc*0.8f;
            float x2r = cx_ + sc, y2r = cy_ - sc*0.4f;
            uk_line(uk, x1r, y1r,
                    x1r + (x2r-x1r)*a2, y1r + (y2r-y1r)*a2, kc, 2.5f*s);
        }
    }

    /* label */
    if (label) {
        TTF_Font *f = uk->font_sm ? uk->font_sm : uk->font;
        uk_text_font(uk, f, label, x + sz + 12*s,
            y + (sz - (float)uk_text_h(f)*s) * 0.5f, th->text);
    }

    uk->cy += row_h;
    return clicked;
}

#endif

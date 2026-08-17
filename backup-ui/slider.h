#ifndef UK_SLIDER_H
#define UK_SLIDER_H

#include "uikit.h"

static inline bool uk_slider(UK *uk, float *val, float lo, float hi, const char *label) {
    float s = uk->scale;
    float x = uk->cx, y = uk->cy;
    float w = uk->cw;
    UK_Theme *th = &uk->th;

    /* label */
    if (label) {
        TTF_Font *f = uk->font_sm ? uk->font_sm : uk->font;
        uk_text_font(uk, f, label, x, y, th->text_dim);
        y += (float)uk_text_h(f) * s + 6 * s;
    }

    float track_h = 6 * s;
    float total_h = track_h + 24 * s;
    float track_y = y + total_h * 0.5f - track_h * 0.5f;

    /* hit area */
    bool hover = uk->mx >= x - 14*s && uk->mx <= x + w + 14*s &&
                 uk->my >= y - 14*s && uk->my <= y + total_h + 14*s;
    UK_WidgetState *st = uk_state(uk, (unsigned)(uintptr_t)val);

    if (hover && uk->mpress) st->dragging = true;
    if (st->dragging) {
        if (uk->mdown) {
            float t = uk_clampf((uk->mx - x) / w, 0, 1);
            *val = lo + (hi - lo) * t;
        } else {
            st->dragging = false;
        }
    }

    float t = uk_clampf((*val - lo) / (hi - lo), 0, 1);

    /* track */
    uk_rect_fill_r(uk, (UK_Rect){ x, track_y, w, track_h },
                    track_h * 0.5f, th->slider_track);
    /* fill */
    float fw = t * w;
    if (fw > 2) {
        uk_rect_fill_r(uk, (UK_Rect){ x, track_y, fw, track_h },
                        track_h * 0.5f, th->slider_fill);
    }

    /* knob */
    float kx = x + fw;
    float ky = track_y + track_h * 0.5f;
    float kr = (8*s) + (st->dragging ? 3*s : hover ? 2*s : 0);

    /* knob shadow */
    uk_circle(uk, kx, ky + 2*s, kr + 2*s, (UK_Color){ 0, 0, 0, 50 });
    /* knob */
    uk_circle(uk, kx, ky, kr, th->slider_knob);
    /* specular on knob */
    uk_circle(uk, kx - kr*0.2f, ky - kr*0.3f, kr*0.35f,
              (UK_Color){ 255, 255, 255, 120 });

    /* value */
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", (int)(t * 100));
    TTF_Font *f = uk->font_xs ? uk->font_xs : uk->font;
    uk_text_font(uk, f, buf, x + w + 10*s,
        ky - (float)uk_text_h(f) * s * 0.5f, th->text_dim);

    uk->cy += total_h + th->spacing * s;
    return st->dragging && uk->mdown;
}

#endif

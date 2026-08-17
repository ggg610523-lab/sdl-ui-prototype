#ifndef UK_RADIO_H
#define UK_RADIO_H

#include "uikit.h"

static inline int uk_radio(UK *uk, int *selected, const char **labels, int count) {
    float s = uk->scale;
    float x = uk->cx, y = uk->cy;
    float w = uk->cw;
    UK_Theme *th = &uk->th;
    float sz = 22 * s;
    int changed = -1;

    for (int i = 0; i < count; i++) {
        float row_h = sz + 10 * s;
        bool hover = uk->mx >= x && uk->mx <= x + w &&
                     uk->my >= y && uk->my <= y + row_h;
        bool clicked = hover && uk->mpress;

        if (clicked && *selected != i) {
            *selected = i;
            changed = i;
        }

        bool active = (*selected == i);
        UK_Color ring_c = active ? th->radio_on_bg : th->radio_off_bg;

        /* outer ring */
        uk_circle(uk, x + sz*0.5f, y + sz*0.5f, sz*0.5f, ring_c);

        /* inner dot */
        if (active) {
            UK_WidgetState *st = uk_state(uk, (unsigned)(uintptr_t)selected + i * 137);
            st->anim += (1.0f - st->anim) * uk->dt * 14;
            if (st->anim > 0.99f) st->anim = 1.0f;
            float dot_r = sz * 0.22f * st->anim;
            uk_circle(uk, x + sz*0.5f, y + sz*0.5f, dot_r, th->radio_knob);
        } else {
            UK_WidgetState *st = uk_state(uk, (unsigned)(uintptr_t)selected + i * 137);
            st->anim = 0;
        }

        /* label */
        if (labels && labels[i]) {
            TTF_Font *f = uk->font_sm ? uk->font_sm : uk->font;
            uk_text_font(uk, f, labels[i], x + sz + 12*s,
                y + (sz - (float)uk_text_h(f)*s) * 0.5f,
                active ? th->text_bright : th->text);
        }

        y += row_h;
    }

    uk->cy = y;
    return changed;
}

#endif

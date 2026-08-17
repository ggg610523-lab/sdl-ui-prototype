#ifndef UK_LABEL_H
#define UK_LABEL_H

#include "uikit.h"

static inline float uk_label(UK *uk, const char *text) {
    float s = uk->scale;
    float lh = (float)uk_text_h(uk->font) * s;
    uk_text(uk, text, uk->cx, uk->cy, uk->th.text);
    float advance = lh + uk->th.spacing * s;
    uk->cy += advance;
    return advance;
}

static inline float uk_label_sm(UK *uk, const char *text) {
    float s = uk->scale;
    float lh = (float)uk_text_h(uk->font_sm) * s;
    uk_text_font(uk, uk->font_sm, text, uk->cx, uk->cy, uk->th.text_dim);
    float advance = lh + uk->th.spacing * s;
    uk->cy += advance;
    return advance;
}

static inline float uk_heading(UK *uk, const char *text) {
    float s = uk->scale;
    float lh = (float)uk_text_h(uk->font_lg) * s;
    uk_text_font(uk, uk->font_lg, text, uk->cx, uk->cy, uk->th.text_bright);
    float advance = lh + uk->th.spacing * s + 8 * s;
    uk->cy += advance;
    return advance;
}

static inline float uk_label_color(UK *uk, const char *text, UK_Color c) {
    float s = uk->scale;
    float lh = (float)uk_text_h(uk->font) * s;
    uk_text(uk, text, uk->cx, uk->cy, c);
    float advance = lh + uk->th.spacing * s;
    uk->cy += advance;
    return advance;
}

static inline float uk_spacing(UK *uk, float h) {
    uk->cy += h * uk->scale;
    return h * uk->scale;
}

#endif

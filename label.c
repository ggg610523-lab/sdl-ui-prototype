#include "label.h"

float uk_heading(
    UK *uk,
    const char *text
) {
    if (!uk || !text)
        return 0.0f;

    TTF_Font *font =
        uk->font_lg ?
        uk->font_lg :
        uk->font;

    float h =
        (float)uk_text_h(font);

    uk_text_font(
        uk,
        font,
        text,
        uk->cx,
        uk->cy,
        uk->th.text
    );

    uk->cy += h + 8.0f;

    return h;
}


float uk_label(
    UK *uk,
    const char *text
) {
    if (!uk || !text)
        return 0.0f;

    float h =
        (float)uk_text_h(uk->font);

    uk_text(
        uk,
        text,
        uk->cx,
        uk->cy,
        uk->th.text
    );

    uk->cy += h + 5.0f;

    return h;
}


float uk_label_sm(
    UK *uk,
    const char *text
) {
    if (!uk || !text)
        return 0.0f;

    TTF_Font *font =
        uk->font_sm ?
        uk->font_sm :
        uk->font;

    float h =
        (float)uk_text_h(font);

    uk_text_font(
        uk,
        font,
        text,
        uk->cx,
        uk->cy,
        uk->th.text_secondary
    );

    uk->cy += h + 6.0f;

    return h;
}


float uk_label_color(
    UK *uk,
    const char *text,
    UK_Color color
) {
    if (!uk || !text)
        return 0.0f;

    float h =
        (float)uk_text_h(uk->font);

    uk_text(
        uk,
        text,
        uk->cx,
        uk->cy,
        color
    );

    uk->cy += h + 5.0f;

    return h;
}

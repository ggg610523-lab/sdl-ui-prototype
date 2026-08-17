#include "checkbox.h"

bool uk_checkbox(
    UK *uk,
    unsigned id,
    const char *text,
    bool *value
) {
    UK_Rect row = {
        uk->cx,
        uk->cy,
        uk->cw,
        34.0f
    };

    bool hit =
        uk_hit(uk, row);

    if (hit && uk->mpress)
        *value = !*value;

    UK_Rect box = {
        row.x,
        row.y + 4.0f,
        25.0f,
        25.0f
    };

    UK_Color bg =
        *value ?
        uk->th.checkbox_on :
        uk->th.checkbox_off;

    uk_rect_fill_r(
        uk,
        box,
        7.0f,
        bg
    );

    if (*value) {
        uk_line(
            uk,
            box.x + 6,
            box.y + 13,
            box.x + 11,
            box.y + 18,
            uk->th.checkbox_check,
            2.0f
        );

        uk_line(
            uk,
            box.x + 11,
            box.y + 18,
            box.x + 20,
            box.y + 7,
            uk->th.checkbox_check,
            2.0f
        );
    }

    uk_text(
        uk,
        text,
        box.x + 36.0f,
        row.y + 6.0f,
        uk->th.text
    );

    uk->cy += 44.0f;

    (void)id;

    return hit && uk->mpress;
}

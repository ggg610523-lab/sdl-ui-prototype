#include "toggle.h"

#include <math.h>

bool uk_toggle(
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

    UK_Rect sw = {
        row.x + row.w - 51.0f,
        row.y + 4.0f,
        51.0f,
        26.0f
    };

    bool hit =
        uk_hit(uk, row);

    if (hit && uk->mpress)
        *value = !*value;

    UK_Color bg =
        *value ?
        uk->th.toggle_on :
        uk->th.toggle_off;

    uk_rect_fill_r(
        uk,
        sw,
        13.0f,
        bg
    );

    float knob_x =
        *value ?
        sw.x + sw.w - 13.0f :
        sw.x + 13.0f;

    uk_circle(
        uk,
        knob_x,
        sw.y + sw.h * 0.5f,
        10.0f,
        uk->th.toggle_knob
    );

    uk_text(
        uk,
        text,
        row.x,
        row.y + 6.0f,
        uk->th.text
    );

    uk->cy += 44.0f;

    (void)id;

    return hit && uk->mpress;
}

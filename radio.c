#include "radio.h"

bool uk_radio(
    UK *uk,
    unsigned id,
    const char *text,
    bool active
) {
    UK_Rect row = {
        uk->cx,
        uk->cy,
        uk->cw,
        34.0f
    };

    bool hit =
        uk_hit(uk, row);

    UK_Rect circle = {
        row.x + 1.0f,
        row.y + 4.0f,
        25.0f,
        25.0f
    };

    float cx =
        circle.x + circle.w * 0.5f;

    float cy =
        circle.y + circle.h * 0.5f;

    uk_circle(
        uk,
        cx,
        cy,
        12.0f,
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
    }

    uk_text(
        uk,
        text,
        row.x + 36.0f,
        row.y + 6.0f,
        uk->th.text
    );

    uk->cy += 44.0f;

    (void)id;

    return hit && uk->mpress;
}

#include "slider.h"

#include <math.h>
#include <stdio.h>

float uk_slider(
    UK *uk,
    unsigned id,
    float value
) {
    UK_WidgetState *st =
        uk_state(uk, id);

    if (!st->initialized) {
        st->initialized = true;
        st->value = value;
    }

    value = uk_clampf(
        value,
        0.0f,
        1.0f
    );

    UK_Rect r = {
        uk->cx,
        uk->cy,
        uk->cw,
        30.0f
    };

    float left = r.x + 7.0f;
    float right = r.x + r.w - 7.0f;

    float center = r.y + r.h * 0.5f;

    UK_Rect track = {
        left,
        center - 3.0f,
        right - left,
        6.0f
    };

    uk_rect_fill_r(
        uk,
        track,
        3.0f,
        uk->th.slider_track
    );

    float t = value;

    if (st->dragging || uk_hit(uk, r)) {
        if (uk->mdown && !st->dragging) {
            st->dragging = true;
        }

        if (st->dragging) {
            t =
                (uk->mx - left) /
                (right - left);

            t = uk_clampf(
                t,
                0.0f,
                1.0f
            );
        }
    }

    if (st->dragging && uk->mrelease)
        st->dragging = false;

    value = t;

    UK_Rect fill = {
        left,
        center - 3.0f,
        (right - left) * t,
        6.0f
    };

    uk_rect_fill_r(
        uk,
        fill,
        3.0f,
        uk->th.slider_fill
    );

    float knob_x =
        left +
        (right - left) * t;

    uk_circle(
        uk,
        knob_x,
        center,
        9.0f,
        uk->th.slider_knob
    );

    uk->cy += 40.0f;

    return value;
}

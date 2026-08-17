#include "uikit.h"
#include <math.h>

static UK_Color panel_default_bg(UK *uk)
{
    UK_Color c = uk->th.glass_bg;

    if (c.a <= 0.0f) {
        c = (UK_Color){
            0.075f,
            0.085f,
            0.105f,
            0.88f
        };
    }

    return c;
}

static UK_Color panel_default_border(UK *uk)
{
    UK_Color c = uk->th.glass_border;

    if (c.a <= 0.0f) {
        c = (UK_Color){
            1.0f,
            1.0f,
            1.0f,
            0.095f
        };
    }

    return c;
}

void uk_panel_begin(
    UK *uk,
    float x,
    float y,
    float w,
    float h
) {
    if (!uk)
        return;

    UK_Rect r = {
        x,
        y,
        w,
        h
    };

    uk->glass_rect = r;

    const float radius = 18.0f;

    /*
     * Large soft shadow.
     *
     * Several transparent layers are used instead of
     * a hard black rectangle. This looks considerably
     * closer to native desktop surfaces.
     */
    for (int i = 4; i >= 1; --i) {
        float spread = (float)i * 2.0f;

        UK_Rect shadow = {
            x - spread * 0.35f,
            y + spread * 0.65f,
            w + spread * 0.7f,
            h + spread * 0.7f
        };

        UK_Color c = {
            0.0f,
            0.0f,
            0.0f,
            0.018f * (float)(5 - i)
        };

        uk_rect_fill_r(
            uk,
            shadow,
            radius + spread,
            c
        );
    }

    /*
     * Main material.
     *
     * No dynamic/light-based renderer here. The panel is
     * intentionally stable and dark.
     */
    UK_Color bg = panel_default_bg(uk);

    uk_rect_fill_r(
        uk,
        r,
        radius,
        bg
    );

    /*
     * Very subtle inner highlight.
     */
    UK_Rect top = {
        x + 1.0f,
        y + 1.0f,
        w - 2.0f,
        1.0f
    };

    uk_rect_fill(
        uk,
        top,
        (UK_Color){
            1.0f,
            1.0f,
            1.0f,
            0.035f
        }
    );

    /*
     * Hairline border.
     */
    uk_rect_stroke_r(
        uk,
        r,
        radius,
        panel_default_border(uk),
        1.0f
    );

    /*
     * Set content layout.
     */
    float pad = 22.0f;

    uk->root_x = x + pad;
    uk->root_y = y + pad;
    uk->root_w = w - pad * 2.0f;

    uk->cx = uk->root_x;
    uk->cy = uk->root_y;
    uk->cw = uk->root_w;
}

void uk_panel_end(UK *uk)
{
    if (!uk)
        return;

    uk->glass_rect = (UK_Rect){0, 0, 0, 0};
}

void uk_panel(
    UK *uk,
    float x,
    float y,
    float w,
    float h
) {
    uk_panel_begin(
        uk,
        x,
        y,
        w,
        h
    );

    uk_panel_end(uk);
}

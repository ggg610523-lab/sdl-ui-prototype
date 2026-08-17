#ifndef UK_PANEL_H
#define UK_PANEL_H

#include "uikit.h"

typedef struct {
    float x;
    float y;
    float w;
    float h;
} UK_Panel;


static inline UK_Panel uk_panel_begin(
    UK *uk,
    float h
) {
    float x = uk->cx;
    float y = uk->cy;
    float w = uk->cw;

    UK_Panel p = {
        x, y, w, h
    };

    uk_glass_begin(
        uk,
        x,
        y,
        w,
        h
    );

    return p;
}


static inline void uk_panel_end(
    UK *uk,
    const UK_Panel *p
) {
    if (!p)
        return;

    /*
     * Continue below the panel.
     */
    uk->cx = p->x;
    uk->cy =
        p->y +
        p->h +
        uk->th.spacing * uk->scale;

    uk->cw = p->w;
}

#endif

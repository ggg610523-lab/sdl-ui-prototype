#ifndef UK_LABEL_H
#define UK_LABEL_H

#include "uikit.h"

float uk_heading(
    UK *uk,
    const char *text
);

float uk_label(
    UK *uk,
    const char *text
);

float uk_label_sm(
    UK *uk,
    const char *text
);

float uk_label_color(
    UK *uk,
    const char *text,
    UK_Color color
);

#endif

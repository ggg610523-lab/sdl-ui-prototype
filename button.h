#ifndef UK_BUTTON_H
#define UK_BUTTON_H

#include "uikit.h"

bool uk_button(
    UK *uk,
    unsigned id,
    const char *text,
    float h
);

bool uk_button_accent(
    UK *uk,
    unsigned id,
    const char *text,
    float h
);

bool uk_button_ghost(
    UK *uk,
    unsigned id,
    const char *text,
    float h
);

#endif

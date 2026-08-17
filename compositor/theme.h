#ifndef THEME_H
#define THEME_H

#include <stdint.h>

#define TITLEBAR_H 42
#define TASKBAR_H  56
#define BORDER_W   1
#define SHADOW_SIZE 8
#define CORNER_R   12

typedef struct
{
    float r, g, b, a;
} ThemeColor;

static const struct
{
    ThemeColor background_top;
    ThemeColor background_bottom;

    ThemeColor panel;
    ThemeColor panel_border;

    ThemeColor titlebar;
    ThemeColor titlebar_active;

    ThemeColor text;
    ThemeColor text_secondary;
    ThemeColor text_muted;

    ThemeColor accent;
    ThemeColor accent_hover;

    ThemeColor button;
    ThemeColor button_hover;

    ThemeColor taskbar;
    ThemeColor taskbar_hover;

    ThemeColor shadow;

    ThemeColor close_active;
    ThemeColor white;
    ThemeColor icon;
} THEME =
{
    { 29.0f/255, 34.0f/255, 44.0f/255, 1.0f },
    { 14.0f/255, 18.0f/255, 25.0f/255, 1.0f },

    { 37.0f/255, 43.0f/255, 54.0f/255, 0.96f },
    { 1.0f, 1.0f, 1.0f, 0.086f },

    { 43.0f/255, 49.0f/255, 61.0f/255, 1.0f },
    { 51.0f/255, 58.0f/255, 71.0f/255, 1.0f },

    { 242.0f/255, 244.0f/255, 248.0f/255, 1.0f },
    { 176.0f/255, 182.0f/255, 194.0f/255, 1.0f },
    { 112.0f/255, 120.0f/255, 135.0f/255, 1.0f },

    { 75.0f/255, 122.0f/255, 224.0f/255, 1.0f },
    { 94.0f/255, 140.0f/255, 238.0f/255, 1.0f },

    { 57.0f/255, 64.0f/255, 77.0f/255, 1.0f },
    { 70.0f/255, 78.0f/255, 93.0f/255, 1.0f },

    { 24.0f/255, 29.0f/255, 37.0f/255, 0.97f },
    { 49.0f/255, 56.0f/255, 68.0f/255, 1.0f },

    { 0.0f, 0.0f, 0.0f, 0.25f },

    { 190.0f/255, 60.0f/255, 67.0f/255, 1.0f },
    { 1.0f, 1.0f, 1.0f, 1.0f },
    { 228.0f/255, 178.0f/255, 65.0f/255, 1.0f },
};

#endif

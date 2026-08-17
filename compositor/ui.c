#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <wlr/types/wlr_buffer.h>
#include <wlr/interfaces/wlr_buffer.h>

#include "server.h"

#ifndef PI
#define PI 3.14159265358979323846
#endif

typedef struct
{
    struct wlr_buffer base;
    uint8_t *data;
    size_t stride;
} PixelBuffer;

static void pixel_buffer_destroy(struct wlr_buffer *buffer)
{
    PixelBuffer *pb = (PixelBuffer *)buffer;
    free(pb->data);
    free(pb);
}

static bool pixel_buffer_begin_access(struct wlr_buffer *buffer,
    uint32_t flags, void **data, uint32_t *format, size_t *stride)
{
    PixelBuffer *pb = (PixelBuffer *)buffer;
    (void)flags;
    *data = pb->data;
    *format = WL_SHM_FORMAT_ARGB8888;
    *stride = pb->stride;
    return true;
}

static void pixel_buffer_end_access(struct wlr_buffer *buffer)
{
    (void)buffer;
}

static const struct wlr_buffer_impl pixel_buffer_impl =
{
    .destroy = pixel_buffer_destroy,
    .begin_data_ptr_access = pixel_buffer_begin_access,
    .end_data_ptr_access = pixel_buffer_end_access,
};

static PixelBuffer *pixel_buffer_create(int width, int height)
{
    PixelBuffer *pb = calloc(1, sizeof(*pb));
    if (!pb) return NULL;

    pb->stride = width * 4;
    pb->data = calloc(1, pb->stride * height);
    if (!pb->data)
    {
        free(pb);
        return NULL;
    }

    wlr_buffer_init(&pb->base, &pixel_buffer_impl, width, height);
    return pb;
}

static cairo_surface_t *pixel_buffer_to_cairo(PixelBuffer *pb)
{
    return cairo_image_surface_create_for_data(
        pb->data,
        CAIRO_FORMAT_ARGB32,
        pb->base.width,
        pb->base.height,
        pb->stride
    );
}

static void draw_rounded_rect(cairo_t *cr,
    double x, double y, double w, double h, double r)
{
    if (r > w * 0.5) r = w * 0.5;
    if (r > h * 0.5) r = h * 0.5;

    cairo_new_sub_path(cr);
    cairo_arc(cr, x + w - r, y + r, r, -PI / 2, 0);
    cairo_arc(cr, x + w - r, y + h - r, r, 0, PI / 2);
    cairo_arc(cr, x + r, y + h - r, r, PI / 2, PI);
    cairo_arc(cr, x + r, y + r, r, PI, PI * 1.5);
    cairo_close_path(cr);
}

static void set_color(cairo_t *cr, ThemeColor c)
{
    cairo_set_source_rgba(cr, c.r, c.g, c.b, c.a);
}

void ui_init_builtins(Server *server)
{
    const char *titles[] = { "Settings", "Files", "About" };
    BuiltinAppID ids[] = { APP_SETTINGS, APP_FILES, APP_ABOUT };

    float positions[][4] =
    {
        { 145, 90, 660, 470 },
        { 250, 210, 450, 390 },
        { 420, 160, 410, 330 },
    };

    for (int i = 0; i < BUILTIN_APP_COUNT; i++)
    {
        BuiltinWindow *w = &server->builtin[i];
        memset(w, 0, sizeof(*w));

        w->id = ids[i];
        snprintf(w->title, sizeof(w->title), "%s", titles[i]);

        w->x = (int)positions[i][0];
        w->y = (int)positions[i][1];
        w->w = (int)positions[i][2];
        w->h = (int)positions[i][3];

        w->target_x = w->x;
        w->target_y = w->y;
        w->target_w = w->w;
        w->target_h = w->h;

        w->normal_x = w->x;
        w->normal_y = w->y;
        w->normal_w = w->w;
        w->normal_h = w->h;

        w->open = true;
        w->z = BUILTIN_APP_COUNT - i;

        w->tree = wlr_scene_tree_create(server->ui_tree);
        w->surface_tree = wlr_scene_tree_create(w->tree);

        wlr_scene_node_set_position(&w->tree->node, w->x, w->y);
    }

    server->builtin[APP_SETTINGS].z = 3;
    server->builtin[APP_FILES].z = 2;
    server->builtin[APP_ABOUT].z = 1;
}

static void render_builtin_titlebar(cairo_t *cr,
    BuiltinWindow *w, bool active, int scale){
    (void)scale;
    int tw = w->w;
    int th = TITLEBAR_H;

    /* titlebar background */
    ThemeColor tc = active ? THEME.titlebar_active : THEME.titlebar;
    set_color(cr, tc);
    draw_rounded_rect(cr, 0, 0, tw, th, CORNER_R);
    cairo_fill(cr);

    /* icon */
    set_color(cr, THEME.accent);
    draw_rounded_rect(cr, 13, 12, 17, 17, 4);
    cairo_fill(cr);

    /* title text */
    PangoLayout *layout = pango_cairo_create_layout(cr);
    pango_layout_set_font_description(layout,
        pango_font_description_from_string("sans 12"));
    pango_layout_set_text(layout, w->title, -1);

    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 40, 11);
    pango_cairo_show_layout(cr, layout);
    g_object_unref(layout);

    /* close button */
    int bx = tw - 120;

    ThemeColor close_bg = active ? THEME.close_active : tc;
    set_color(cr, close_bg);
    cairo_rectangle(cr, bx + 80, 0, 40, th);
    cairo_fill(cr);

    /* close X */
    float cx = bx + 80 + 20;
    float cy = th * 0.5f;
    cairo_set_line_width(cr, 1.5);
    cairo_set_source_rgba(cr,
        THEME.white.r, THEME.white.g, THEME.white.b, THEME.white.a);
    cairo_move_to(cr, cx - 6, cy - 6);
    cairo_line_to(cr, cx + 6, cy + 6);
    cairo_move_to(cr, cx + 6, cy - 6);
    cairo_line_to(cr, cx - 6, cy + 6);
    cairo_stroke(cr);

    /* maximize button */
    set_color(cr, tc);
    cairo_rectangle(cr, bx + 40, 0, 40, th);
    cairo_fill(cr);

    float mx = bx + 40 + 12;
    float my = 12;
    cairo_set_line_width(cr, 1.5);
    cairo_set_source_rgba(cr,
        THEME.white.r, THEME.white.g, THEME.white.b, THEME.white.a);
    cairo_rectangle(cr, mx, my, 16, 15);
    cairo_stroke(cr);

    /* minimize button */
    set_color(cr, tc);
    cairo_rectangle(cr, bx, 0, 40, th);
    cairo_fill(cr);

    float mnx = bx + 12;
    float mny = th * 0.5f;
    cairo_set_line_width(cr, 2.0);
    cairo_set_source_rgba(cr,
        THEME.white.r, THEME.white.g, THEME.white.b, THEME.white.a);
    cairo_move_to(cr, mnx, mny);
    cairo_line_to(cr, mnx + 16, mny);
    cairo_stroke(cr);
}

static void render_settings_content(cairo_t *cr, int w, int h)
{
    (void)h;
    int content_y = TITLEBAR_H + 25;

    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *font = pango_font_description_from_string("sans 18");
    PangoFontDescription *small = pango_font_description_from_string("sans 12");

    /* title */
    pango_layout_set_font_description(layout, font);
    pango_layout_set_text(layout, "Settings", -1);
    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 30, content_y);
    pango_cairo_show_layout(cr, layout);

    /* subtitle */
    pango_layout_set_font_description(layout, small);
    pango_layout_set_text(layout, "Personalize your desktop experience.", -1);
    cairo_set_source_rgba(cr,
        THEME.text_secondary.r, THEME.text_secondary.g,
        THEME.text_secondary.b, THEME.text_secondary.a);
    cairo_move_to(cr, 30, content_y + 32);
    pango_cairo_show_layout(cr, layout);

    /* volume label */
    pango_layout_set_text(layout, "Volume", -1);
    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 30, content_y + 80);
    pango_cairo_show_layout(cr, layout);

    /* volume track */
    int track_y = content_y + 111;
    int track_w = w - 60;

    set_color(cr, (ThemeColor){ 90.0f/255, 98.0f/255, 112.0f/255, 0.6f });
    draw_rounded_rect(cr, 30, track_y, track_w, 7, 4);
    cairo_fill(cr);

    set_color(cr, THEME.accent);
    draw_rounded_rect(cr, 30, track_y, track_w * 0.68f, 7, 4);
    cairo_fill(cr);

    /* volume knob */
    cairo_arc(cr, 30 + track_w * 0.68f, track_y + 3.5f, 10, 0, PI * 2);
    cairo_set_source_rgba(cr, 0.97, 0.98, 0.99, 1.0);
    cairo_fill(cr);

    /* animations switch */
    pango_layout_set_text(layout, "Animations", -1);
    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 30, content_y + 165);
    pango_cairo_show_layout(cr, layout);

    /* switch track */
    int sw_x = w - 88;
    int sw_y = content_y + 155;
    set_color(cr, THEME.accent);
    draw_rounded_rect(cr, sw_x, sw_y, 54, 30, 15);
    cairo_fill(cr);

    /* switch knob */
    cairo_arc(cr, sw_x + 54 - 15, sw_y + 15, 10, 0, PI * 2);
    cairo_set_source_rgba(cr, 0.98, 0.985, 0.99, 1.0);
    cairo_fill(cr);

    /* theme label */
    pango_layout_set_text(layout, "Theme", -1);
    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 30, content_y + 225);
    pango_cairo_show_layout(cr, layout);

    /* radio buttons */
    const char *names[] = { "Dark", "Light", "System" };
    for (int i = 0; i < 3; i++)
    {
        int ry = content_y + 265 + i * 42;
        int cx = 43;
        int cy = ry + 8;

        /* outer circle */
        cairo_arc(cr, cx, cy, 9, 0, PI * 2);
        set_color(cr, (ThemeColor){ 90.0f/255, 98.0f/255, 112.0f/255, 1.0 });
        cairo_fill(cr);

        if (i == 0)
        {
            cairo_arc(cr, cx, cy, 5, 0, PI * 2);
            set_color(cr, THEME.accent);
            cairo_fill(cr);
        }

        pango_layout_set_text(layout, names[i], -1);
        cairo_set_source_rgba(cr,
            THEME.text_secondary.r, THEME.text_secondary.g,
            THEME.text_secondary.b, THEME.text_secondary.a);
        cairo_move_to(cr, 62, ry);
        pango_cairo_show_layout(cr, layout);
    }

    g_object_unref(layout);
}

static void render_files_content(cairo_t *cr, int w, int h)
{
    (void)h;
    int content_y = TITLEBAR_H + 25;

    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *font = pango_font_description_from_string("sans 18");
    PangoFontDescription *small = pango_font_description_from_string("sans 12");

    /* title */
    pango_layout_set_font_description(layout, font);
    pango_layout_set_text(layout, "Files", -1);
    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 30, content_y);
    pango_cairo_show_layout(cr, layout);

    const char *items[] =
    {
        "Desktop", "Documents", "Downloads", "Pictures", "Music"
    };

    pango_layout_set_font_description(layout, small);

    for (int i = 0; i < 5; i++)
    {
        int iy = content_y + 45 + i * 48;

        /* row background */
        ThemeColor bg = (i == 0) ? THEME.button_hover : THEME.button;
        set_color(cr, bg);
        draw_rounded_rect(cr, 18, iy - 7, w - 36, 39, 9);
        cairo_fill(cr);

        /* folder icon */
        set_color(cr, THEME.icon);
        draw_rounded_rect(cr, 30, iy + 4, 20, 15, 3);
        cairo_fill(cr);

        /* text */
        pango_layout_set_text(layout, items[i], -1);
        cairo_set_source_rgba(cr,
            THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
        cairo_move_to(cr, 62, iy);
        pango_cairo_show_layout(cr, layout);
    }

    g_object_unref(layout);
}

static void render_about_content(cairo_t *cr, int w, int h)
{
    (void)w; (void)h;
    int content_y = TITLEBAR_H + 30;

    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *font = pango_font_description_from_string("sans 18");
    PangoFontDescription *small = pango_font_description_from_string("sans 12");

    /* icon */
    set_color(cr, THEME.accent);
    draw_rounded_rect(cr, 30, content_y, 54, 54, 14);
    cairo_fill(cr);

    pango_layout_set_font_description(layout, font);
    pango_layout_set_text(layout, "UI", -1);
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_move_to(cr, 43, content_y + 13);
    pango_cairo_show_layout(cr, layout);

    /* title */
    pango_layout_set_text(layout, "SDL Desktop", -1);
    cairo_set_source_rgba(cr,
        THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
    cairo_move_to(cr, 30, content_y + 75);
    pango_cairo_show_layout(cr, layout);

    /* subtitle */
    pango_layout_set_font_description(layout, small);
    pango_layout_set_text(layout, "A native Wayland compositor.", -1);
    cairo_set_source_rgba(cr,
        THEME.text_secondary.r, THEME.text_secondary.g,
        THEME.text_secondary.b, THEME.text_secondary.a);
    cairo_move_to(cr, 30, content_y + 110);
    pango_cairo_show_layout(cr, layout);

    pango_layout_set_text(layout, "Smooth windows, controls and scaling.", -1);
    cairo_set_source_rgba(cr,
        THEME.text_muted.r, THEME.text_muted.g,
        THEME.text_muted.b, THEME.text_muted.a);
    cairo_move_to(cr, 30, content_y + 145);
    pango_cairo_show_layout(cr, layout);

    /* OK button */
    set_color(cr, THEME.accent);
    draw_rounded_rect(cr, 30, content_y + 185, 130, 42, 11);
    cairo_fill(cr);

    pango_layout_set_font_description(layout, small);
    pango_layout_set_text(layout, "OK", -1);
    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
    cairo_move_to(cr, 83, content_y + 198);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
}

static void update_builtin_window(Server *server, BuiltinWindow *w)
{
    (void)server;
    if (!w->open || w->minimized)
    {
        wlr_scene_node_set_enabled(&w->tree->node, false);
        return;
    }

    wlr_scene_node_set_enabled(&w->tree->node, true);

    int pw = w->w;
    int ph = w->h;

    if (pw < 10 || ph < 10) return;

    PixelBuffer *pb = pixel_buffer_create(pw, ph);
    if (!pb) return;

    cairo_surface_t *surf = pixel_buffer_to_cairo(pb);
    cairo_t *cr = cairo_create(surf);

    /* body */
    set_color(cr, THEME.panel);
    draw_rounded_rect(cr, 0, 0, pw, ph, CORNER_R);
    cairo_fill(cr);

    /* content */
    cairo_save(cr);
    cairo_translate(cr, 0, 0);

    switch (w->id)
    {
        case APP_SETTINGS:
            render_settings_content(cr, pw, ph);
            break;
        case APP_FILES:
            render_files_content(cr, pw, ph);
            break;
        case APP_ABOUT:
            render_about_content(cr, pw, ph);
            break;
    }

    cairo_restore(cr);

    /* titlebar */
    render_builtin_titlebar(cr, w, true, 1);

    cairo_destroy(cr);
    cairo_surface_destroy(surf);

    wlr_scene_buffer_set_buffer(w->titlebar_buffer, &pb->base);
    wlr_buffer_drop(&pb->base);

    wlr_scene_node_set_position(&w->tree->node, w->x, w->y);
}

void ui_init_taskbar(Server *server)
{
    int screen_w = server->output.width;
    int screen_h = server->output.height;

    server->taskbar_bg = wlr_scene_rect_create(
        server->ui_tree,
        screen_w, TASKBAR_H,
        (float[]){ THEME.taskbar.r, THEME.taskbar.g,
                   THEME.taskbar.b, THEME.taskbar.a }
    );
    wlr_scene_node_set_position(
        &server->taskbar_bg->node,
        0, screen_h - TASKBAR_H
    );

    int tb_w = screen_w;
    int tb_h = TASKBAR_H;
    PixelBuffer *pb = pixel_buffer_create(tb_w, tb_h);
    if (!pb) return;

    cairo_surface_t *surf = pixel_buffer_to_cairo(pb);
    cairo_t *cr = cairo_create(surf);

    cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
    cairo_paint(cr);
    cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

    /* start button */
    set_color(cr, THEME.button);
    draw_rounded_rect(cr, 14, 10, 36, 36, 8);
    cairo_fill(cr);

    /* 4-square icon */
    float sx = 23;
    float sy = 19;
    float s = 7;
    cairo_set_source_rgba(cr, 0.88, 0.89, 0.92, 1.0);
    cairo_rectangle(cr, sx, sy, s, s);
    cairo_fill(cr);
    cairo_rectangle(cr, sx + s + 2, sy, s, s);
    cairo_fill(cr);
    cairo_rectangle(cr, sx, sy + s + 2, s, s);
    cairo_fill(cr);
    cairo_rectangle(cr, sx + s + 2, sy + s + 2, s, s);
    cairo_fill(cr);

    /* app buttons */
    PangoLayout *layout = pango_cairo_create_layout(cr);
    PangoFontDescription *small = pango_font_description_from_string("sans 11");
    pango_layout_set_font_description(layout, small);

    float bx = 65;
    for (int i = 0; i < BUILTIN_APP_COUNT; i++)
    {
        BuiltinWindow *w = &server->builtin[i];

        set_color(cr, THEME.taskbar);
        draw_rounded_rect(cr, bx, 8, 155, 42, 8);
        cairo_fill(cr);

        /* app icon */
        set_color(cr, THEME.accent);
        draw_rounded_rect(cr, bx + 10, 18, 16, 16, 4);
        cairo_fill(cr);

        /* app title */
        cairo_set_source_rgba(cr,
            THEME.text.r, THEME.text.g, THEME.text.b, THEME.text.a);
        pango_layout_set_text(layout, w->title, -1);
        cairo_move_to(cr, bx + 34, 20);
        pango_cairo_show_layout(cr, layout);

        bx += 165;
    }

    /* brand text */
    cairo_set_source_rgba(cr,
        THEME.text_secondary.r, THEME.text_secondary.g,
        THEME.text_secondary.b, THEME.text_secondary.a);
    pango_layout_set_text(layout, "Wayland Desktop", -1);
    cairo_move_to(cr, tb_w - 130, 20);
    pango_cairo_show_layout(cr, layout);

    g_object_unref(layout);
    cairo_destroy(cr);
    cairo_surface_destroy(surf);

    server->taskbar_buffer = wlr_scene_buffer_create(
        server->ui_tree, &pb->base
    );
    wlr_scene_node_set_position(
        &server->taskbar_buffer->node,
        0, screen_h - TASKBAR_H
    );
    wlr_buffer_drop(&pb->base);
}

void ui_init(Server *server)
{
    int screen_w = server->output.width;
    int screen_h = server->output.height;

    /* background */
    struct wlr_scene_tree *bg_tree = wlr_scene_tree_create(
        &server->scene->tree    );

    int bands = 20;
    for (int i = 0; i < bands; i++)
    {
        float t = (float)i / (float)(bands - 1);
        float r = THEME.background_top.r * (1 - t) +
                  THEME.background_bottom.r * t;
        float g = THEME.background_top.g * (1 - t) +
                  THEME.background_bottom.g * t;
        float b = THEME.background_top.b * (1 - t) +
                  THEME.background_bottom.b * t;

        int band_h = screen_h / bands + 1;
        struct wlr_scene_rect *rect = wlr_scene_rect_create(
            bg_tree,
            screen_w, band_h,
            (float[]){ r, g, b, 1.0f }
        );
        wlr_scene_node_set_position(
            &rect->node,
            0, i * (screen_h / bands)
        );
    }

    /* UI tree on top of clients */
    server->ui_tree = wlr_scene_tree_create(&server->scene->tree);

    /* taskbar */
    ui_init_taskbar(server);

    /* builtin app windows */
    ui_init_builtins(server);

    for (int i = 0; i < BUILTIN_APP_COUNT; i++)
    {
        BuiltinWindow *w = &server->builtin[i];
        w->titlebar_buffer = wlr_scene_buffer_create(
            w->tree, NULL
        );

        wlr_scene_node_set_position(&w->tree->node, w->x, w->y);
        update_builtin_window(server, w);
    }
}

void ui_update_builtins(Server *server)
{
    for (int i = 0; i < BUILTIN_APP_COUNT; i++)
    {
        BuiltinWindow *w = &server->builtin[i];

        if (!w->open || w->minimized)
        {
            wlr_scene_node_set_enabled(&w->tree->node, false);
            continue;
        }

        wlr_scene_node_set_enabled(&w->tree->node, true);
        update_builtin_window(server, w);
    }
}

int ui_builtin_at(Server *server, double lx, double ly)
{
    for (int i = BUILTIN_APP_COUNT - 1; i >= 0; i--)
    {
        BuiltinWindow *w = &server->builtin[i];
        if (!w->open || w->minimized) continue;

        if (lx >= w->x && lx <= w->x + w->w &&
            ly >= w->y && ly <= w->y + w->h)
        {
            return i;
        }
    }
    return -1;
}

int ui_builtin_button_at(Server *server, int id, double lx, double ly)
{
    BuiltinWindow *w = &server->builtin[id];
    if (!w->open || w->minimized) return 0;

    if (ly < w->y || ly > w->y + TITLEBAR_H) return 0;

    int right = w->x + w->w;

    if (lx >= right - 40 && lx <= right)
        return -1;
    if (lx >= right - 80 && lx <= right - 40)
        return -2;
    if (lx >= right - 120 && lx <= right - 80)
        return -3;

    return 0;
}

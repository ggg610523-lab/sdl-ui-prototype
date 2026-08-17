#ifndef SERVER_H
#define SERVER_H

#ifndef WLR_USE_UNSTABLE
#error "Add -DWLR_USE_UNSTABLE to enable unstable wlroots features"
#endif

#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <xkbcommon/xkbcommon.h>

#include "theme.h"

#define BUILTIN_APP_COUNT 3

typedef enum
{
    APP_SETTINGS,
    APP_FILES,
    APP_ABOUT,
} BuiltinAppID;

typedef struct
{
    struct wlr_scene_tree *tree;
    struct wlr_scene_buffer *titlebar_buffer;
    struct wlr_scene_tree *surface_tree;

    int x, y;
    int w, h;

    int target_x, target_y;
    int target_w, target_h;

    int normal_x, normal_y;
    int normal_w, normal_h;

    bool open;
    bool minimized;
    bool maximized;
    bool dragging;

    float drag_offset_x;
    float drag_offset_y;
    int z;

    BuiltinAppID id;
    char title[64];
} BuiltinWindow;

typedef struct Server Server;

struct Server
{
    struct wl_display *display;
    struct wlr_backend *backend;
    struct wlr_renderer *renderer;
    struct wlr_allocator *allocator;
    struct wlr_scene *scene;
    struct wlr_scene_output_layout *scene_layout;
    struct wlr_output_layout *output_layout;

    struct wlr_xdg_shell *xdg_shell;
    struct wlr_xdg_decoration_manager_v1 *deco_manager;
    struct wlr_compositor *compositor;

    struct wlr_seat *seat;
    struct wlr_cursor *cursor;
    struct wlr_xcursor_manager *cursor_mgr;

    struct wlr_scene_tree *client_tree;
    struct wlr_scene_tree *ui_tree;

    struct wlr_scene_rect *taskbar_bg;
    struct wlr_scene_buffer *taskbar_buffer;

    struct wl_list clients;

    struct wl_listener backend_new_input;
    struct wl_listener backend_new_output;
    struct wl_listener xdg_shell_new_surface;
    struct wl_listener deco_new_toplevel;
    struct wl_listener seat_request_set_cursor;

    BuiltinWindow builtin[BUILTIN_APP_COUNT];

    struct
    {
        struct wlr_keyboard *keyboard;
        struct wl_listener modifiers;
        struct wl_listener key;
        struct wl_listener destroy;
    } kb;

    struct
    {
        struct wlr_scene_tree *tree;
    } cursor_scene;

    struct
    {
        struct wl_listener request_set_cursor;
    } seat_listeners;

    struct
    {
        uint32_t width;
        uint32_t height;
        struct wlr_scene_output *scene_output;
    } output;
};

typedef struct Client Client;

struct Client
{
    struct wl_list link;

    Server *server;

    struct wlr_xdg_surface *xdg_surface;
    struct wlr_xdg_toplevel *toplevel;
    struct wlr_xdg_toplevel_decoration_v1 *decoration;

    struct wlr_scene_tree *tree;
    struct wlr_scene_tree *surface_tree;
    struct wlr_scene_buffer *deco_buffer;

    struct wl_listener surface_map;
    struct wl_listener surface_unmap;
    struct wl_listener toplevel_request_maximize;
    struct wl_listener toplevel_request_fullscreen;
    struct wl_listener toplevel_request_minimize;
    struct wl_listener toplevel_request_move;
    struct wl_listener toplevel_request_resize;
    struct wl_listener toplevel_destroy;
    struct wl_listener toplevel_set_title;
    struct wl_listener toplevel_set_app_id;
    struct wl_listener deco_request_mode;
    struct wl_listener deco_destroy;
    struct wl_listener new_popup;
    struct wl_listener xdg_destroy;

    bool mapped;
    bool ssd;
};

typedef struct
{
    double x, y;
    uint32_t width, height;
    enum
    {
        RESIZE_EDGE_NONE = 0,
        RESIZE_EDGE_LEFT = 1,
        RESIZE_EDGE_RIGHT = 2,
        RESIZE_EDGE_TOP = 4,
        RESIZE_EDGE_BOTTOM = 8,
    } resize_edge;
    Client *client;
    BuiltinWindow *builtin;
} CursorState;

void server_init(Server *server);
void server_run(Server *server);
void server_destroy(Server *server);

#endif

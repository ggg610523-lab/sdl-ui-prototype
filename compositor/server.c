#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

extern void xdg_shell_init(Server *server);
extern void input_init(Server *server);
extern void ui_init(Server *server);

void server_init(Server *server)
{
    memset(server, 0, sizeof(*server));

    server->display = wl_display_create();
    if (!server->display)
    {
        fprintf(stderr, "Failed to create display\n");
        exit(1);
    }

    server->backend = wlr_backend_autocreate(
        wl_display_get_event_loop(server->display),
        NULL
    );
    if (!server->backend)
    {
        fprintf(stderr, "Failed to create backend\n");
        wl_display_destroy(server->display);
        exit(1);
    }

    server->renderer = wlr_renderer_autocreate(server->backend);
    if (!server->renderer)
    {
        fprintf(stderr, "Failed to create renderer\n");
        wlr_backend_destroy(server->backend);
        wl_display_destroy(server->display);
        exit(1);
    }

    server->allocator = wlr_allocator_autocreate(
        server->backend,
        server->renderer
    );

    server->compositor = wlr_compositor_create(
        server->display, 5, server->renderer
    );

    wlr_data_device_manager_create(server->display);

    server->scene = wlr_scene_create();
    server->output_layout = wlr_output_layout_create(server->display);
    server->scene_layout = wlr_scene_attach_output_layout(
        server->scene, server->output_layout
    );

    server->seat = wlr_seat_create(server->display, "seat0");

    wl_list_init(&server->clients);

    xdg_shell_init(server);
    input_init(server);
}

void server_run(Server *server)
{
    wlr_backend_start(server->backend);

    fprintf(stderr, "Desktop started. Running event loop...\n");

    ui_init(server);

    wl_display_run(server->display);
}

void server_destroy(Server *server)
{
    wl_display_destroy_clients(server->display);

    wl_list_remove(&server->seat_listeners.request_set_cursor.link);
    wl_list_remove(&server->backend_new_input.link);
    wl_list_remove(&server->backend_new_output.link);
    wl_list_remove(&server->xdg_shell_new_surface.link);
    wl_list_remove(&server->deco_new_toplevel.link);

    wlr_scene_node_destroy(&server->scene->tree.node);
    wlr_output_layout_destroy(server->output_layout);
    wlr_seat_destroy(server->seat);
    wlr_xcursor_manager_destroy(server->cursor_mgr);
    wlr_cursor_destroy(server->cursor);

    wlr_allocator_destroy(server->allocator);
    wlr_renderer_destroy(server->renderer);
    wlr_backend_destroy(server->backend);

    wl_display_destroy(server->display);
}

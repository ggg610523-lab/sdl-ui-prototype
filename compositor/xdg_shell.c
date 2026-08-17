#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

static void xdg_surface_map(struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(listener, client, surface_map);
    (void)data;

    client->mapped = true;
    wlr_scene_node_set_enabled(&client->tree->node, true);
}

static void xdg_surface_unmap(struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(listener, client, surface_unmap);
    (void)data;

    client->mapped = false;
    wlr_scene_node_set_enabled(&client->tree->node, false);
}

static void xdg_toplevel_request_maximize(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, toplevel_request_maximize
    );
    (void)data;

    if (!client->toplevel) return;

    wlr_xdg_toplevel_set_maximized(client->toplevel,
        !client->toplevel->current.maximized);
}

static void xdg_toplevel_request_fullscreen(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, toplevel_request_fullscreen
    );
    (void)data;

    if (!client->toplevel) return;

    wlr_xdg_toplevel_set_fullscreen(client->toplevel,
        !client->toplevel->current.fullscreen);
}

static void xdg_toplevel_request_minimize(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, toplevel_request_minimize
    );
    (void)data;

    if (!client->mapped) return;

    wlr_scene_node_set_enabled(&client->tree->node, false);
    client->mapped = false;
}

static void xdg_toplevel_request_move(
    struct wl_listener *listener, void *data)
{
    (void)listener;
    (void)data;
}

static void xdg_toplevel_request_resize(
    struct wl_listener *listener, void *data)
{
    (void)listener;
    (void)data;
}

static void xdg_toplevel_destroy(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, toplevel_destroy
    );
    (void)data;

    client->toplevel = NULL;
}

static void xdg_toplevel_set_title(
    struct wl_listener *listener, void *data)
{
    (void)listener;
    (void)data;
}

static void xdg_toplevel_set_app_id(
    struct wl_listener *listener, void *data)
{
    (void)listener;
    (void)data;
}

static void xdg_decoration_request_mode(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, deco_request_mode
    );
    (void)data;

    if (!client->decoration) return;

    wlr_xdg_toplevel_decoration_v1_set_mode(
        client->decoration,
        WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
    );
}

static void xdg_decoration_destroy(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, deco_destroy
    );
    (void)data;

    client->decoration = NULL;
}

static void xdg_new_popup(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, new_popup
    );
    struct wlr_xdg_popup *popup = data;

    struct wlr_scene_tree *tree =
        wlr_scene_xdg_surface_create(
            client->surface_tree, popup->base
    );
    (void)tree;
}

static void xdg_surface_destroy_handler(
    struct wl_listener *listener, void *data)
{
    Client *client = wl_container_of(
        listener, client, xdg_destroy
    );
    (void)data;

    wl_list_remove(&client->link);

    wl_list_remove(&client->surface_map.link);
    wl_list_remove(&client->surface_unmap.link);
    wl_list_remove(&client->toplevel_request_maximize.link);
    wl_list_remove(&client->toplevel_request_fullscreen.link);
    wl_list_remove(&client->toplevel_request_minimize.link);
    wl_list_remove(&client->toplevel_request_move.link);
    wl_list_remove(&client->toplevel_request_resize.link);
    wl_list_remove(&client->xdg_destroy.link);
    wl_list_remove(&client->new_popup.link);
    wl_list_remove(&client->toplevel_destroy.link);
    wl_list_remove(&client->toplevel_set_title.link);
    wl_list_remove(&client->toplevel_set_app_id.link);

    if (client->decoration)
    {
        wl_list_remove(&client->deco_request_mode.link);
        wl_list_remove(&client->deco_destroy.link);
    }

    free(client);
}

static void handle_xdg_surface(Server *server,
    struct wlr_xdg_surface *xdg_surface)
{
    if (xdg_surface->role != WLR_XDG_SURFACE_ROLE_TOPLEVEL)
        return;

    struct wlr_xdg_toplevel *toplevel = xdg_surface->toplevel;
    if (!toplevel) return;

    Client *client = calloc(1, sizeof(*client));
    if (!client) return;

    client->server = server;
    client->xdg_surface = xdg_surface;
    client->toplevel = toplevel;
    client->ssd = true;

    xdg_surface->data = client;

    client->tree = wlr_scene_tree_create(server->client_tree);
    client->surface_tree = wlr_scene_xdg_surface_create(
        client->tree, xdg_surface
    );

    wlr_scene_node_set_enabled(&client->tree->node, false);

    client->deco_buffer = wlr_scene_buffer_create(
        client->tree, NULL
    );
    wlr_scene_node_set_position(
        &client->deco_buffer->node, 0, 0
    );

    /* map/unmap from the underlying wlr_surface */
    client->surface_map.notify = xdg_surface_map;
    wl_signal_add(&xdg_surface->surface->events.map,
        &client->surface_map);

    client->surface_unmap.notify = xdg_surface_unmap;
    wl_signal_add(&xdg_surface->surface->events.unmap,
        &client->surface_unmap);

    client->xdg_destroy.notify = xdg_surface_destroy_handler;
    wl_signal_add(&xdg_surface->events.destroy, &client->xdg_destroy);

    client->new_popup.notify = xdg_new_popup;
    wl_signal_add(&xdg_surface->events.new_popup, &client->new_popup);

    client->toplevel_destroy.notify = xdg_toplevel_destroy;
    wl_signal_add(&toplevel->events.destroy, &client->toplevel_destroy);

    client->toplevel_request_maximize.notify =
        xdg_toplevel_request_maximize;
    wl_signal_add(&toplevel->events.request_maximize,
        &client->toplevel_request_maximize);

    client->toplevel_request_fullscreen.notify =
        xdg_toplevel_request_fullscreen;
    wl_signal_add(&toplevel->events.request_fullscreen,
        &client->toplevel_request_fullscreen);

    client->toplevel_request_minimize.notify =
        xdg_toplevel_request_minimize;
    wl_signal_add(&toplevel->events.request_minimize,
        &client->toplevel_request_minimize);

    client->toplevel_request_move.notify =
        xdg_toplevel_request_move;
    wl_signal_add(&toplevel->events.request_move,
        &client->toplevel_request_move);

    client->toplevel_request_resize.notify =
        xdg_toplevel_request_resize;
    wl_signal_add(&toplevel->events.request_resize,
        &client->toplevel_request_resize);

    client->toplevel_set_title.notify = xdg_toplevel_set_title;
    wl_signal_add(&toplevel->events.set_title,
        &client->toplevel_set_title);

    client->toplevel_set_app_id.notify = xdg_toplevel_set_app_id;
    wl_signal_add(&toplevel->events.set_app_id,
        &client->toplevel_set_app_id);

    fprintf(stderr, "New client: %s\n",
        toplevel->title ? toplevel->title : "Window");

    wl_list_insert(&server->clients, &client->link);

    wlr_xdg_toplevel_set_size(toplevel, 0, 0);
}

static void xdg_shell_new_surface(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, xdg_shell_new_surface
    );
    struct wlr_xdg_surface *surface = data;

    handle_xdg_surface(server, surface);
}

static void deco_new_toplevel(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, deco_new_toplevel
    );
    struct wlr_xdg_toplevel_decoration_v1 *decoration = data;

    struct wlr_xdg_toplevel *toplevel = decoration->toplevel;
    if (!toplevel || !toplevel->base || !toplevel->base->data)
        return;

    Client *client = toplevel->base->data;

    client->decoration = decoration;

    client->deco_request_mode.notify = xdg_decoration_request_mode;
    wl_signal_add(&decoration->events.request_mode,
        &client->deco_request_mode);

    client->deco_destroy.notify = xdg_decoration_destroy;
    wl_signal_add(&decoration->events.destroy,
        &client->deco_destroy);

    wlr_xdg_toplevel_decoration_v1_set_mode(
        decoration,
        WLR_XDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE
    );
}

void xdg_shell_init(Server *server)
{
    server->xdg_shell = wlr_xdg_shell_create(server->display, 3);

    server->xdg_shell_new_surface.notify = xdg_shell_new_surface;
    wl_signal_add(&server->xdg_shell->events.new_surface,
        &server->xdg_shell_new_surface);

    server->deco_manager =
        wlr_xdg_decoration_manager_v1_create(server->display);

    server->deco_new_toplevel.notify = deco_new_toplevel;
    wl_signal_add(
        &server->deco_manager->events.new_toplevel_decoration,
        &server->deco_new_toplevel
    );

    server->client_tree = wlr_scene_tree_create(&server->scene->tree);
}

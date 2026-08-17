#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "server.h"

static void keyboard_handleModifiers(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, kb.modifiers
    );
    (void)data;

    struct wlr_keyboard *keyboard = server->kb.keyboard;
    if (!keyboard) return;

    wlr_seat_set_keyboard(server->seat, keyboard);
    wlr_seat_keyboard_notify_modifiers(
        server->seat, &keyboard->modifiers
    );
}

static Client *server_focused_client(Server *server)
{
    struct wlr_seat_client *seat_client =
        server->seat->keyboard_state.focused_client;
    if (!seat_client) return NULL;

    struct wlr_surface *focus_surface =
        server->seat->keyboard_state.focused_surface;

    Client *client;
    wl_list_for_each(client, &server->clients, link)
    {
        if (client->xdg_surface &&
            client->xdg_surface->surface == focus_surface)
        {
            return client;
        }
    }
    return NULL;
}

static void focus_client(Server *server, Client *client)
{
    if (!client)
    {
        wlr_seat_keyboard_notify_clear_focus(server->seat);
        return;
    }

    struct wlr_surface *surface =
        client->xdg_surface->surface;

    wlr_seat_keyboard_notify_enter(
        server->seat,
        surface,
        server->kb.keyboard->keycodes,
        server->kb.keyboard->num_keycodes,
        &server->kb.keyboard->modifiers
    );
}

static void keyboard_handleKey(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, kb.key
    );
    struct wlr_keyboard_key_event *event = data;

    struct wlr_keyboard *keyboard = server->kb.keyboard;
    if (!keyboard) return;

    xkb_state_update_key(
        keyboard->xkb_state,
        event->keycode + 8,
        event->state == WL_KEYBOARD_KEY_STATE_PRESSED ?
            XKB_KEY_DOWN : XKB_KEY_UP
    );

    xkb_mod_mask_t mods = xkb_state_serialize_mods(
        keyboard->xkb_state,
        XKB_STATE_MODS_EFFECTIVE
    );

    bool handled = false;

    if (event->state == WL_KEYBOARD_KEY_STATE_PRESSED)
    {
        xkb_keysym_t sym = xkb_state_key_get_one_sym(
            keyboard->xkb_state, event->keycode + 8
        );

        if (sym == XKB_KEY_F11)
        {
            Client *focused = server_focused_client(server);
            if (focused && focused->toplevel)
            {
                wlr_xdg_toplevel_set_fullscreen(
                    focused->toplevel,
                    !focused->toplevel->current.fullscreen
                );
            }
            handled = true;
        }

        if ((mods & WLR_MODIFIER_ALT) && sym == XKB_KEY_Tab)
        {
            Client *focused = server_focused_client(server);
            Client *next = NULL;

            if (focused)
            {
                struct wl_list *it = focused->link.next;
                while (it != &server->clients)
                {
                    Client *c = wl_container_of(it, c, link);
                    if (c->mapped)
                    {
                        next = c;
                        break;
                    }
                    it = it->next;
                }
            }

            if (!next)
            {
                struct wl_list *it = server->clients.next;
                while (it != &server->clients)
                {
                    Client *c = wl_container_of(it, c, link);
                    if (c->mapped)
                    {
                        next = c;
                        break;
                    }
                    it = it->next;
                }
            }

            if (next)
                focus_client(server, next);

            handled = true;
        }

        if ((mods & WLR_MODIFIER_ALT) && sym == XKB_KEY_q)
        {
            Client *focused = server_focused_client(server);
            if (focused && focused->toplevel)
                wlr_xdg_toplevel_send_close(focused->toplevel);
            handled = true;
        }
    }

    if (!handled)
    {
        wlr_seat_set_keyboard(server->seat, keyboard);
        wlr_seat_keyboard_notify_key(
            server->seat,
            event->time_msec,
            event->keycode,
            event->state
        );
    }
}

static void keyboard_destroy(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, kb.destroy
    );
    (void)data;

    server->kb.keyboard = NULL;
    wl_list_remove(&server->kb.modifiers.link);
    wl_list_remove(&server->kb.key.link);
    wl_list_remove(&server->kb.destroy.link);
}

static void seat_request_set_cursor(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, seat_listeners.request_set_cursor
    );
    struct wlr_seat_pointer_request_set_cursor_event *event = data;

    struct wlr_seat_client *seat_client =
        server->seat->pointer_state.focused_client;

    if (event->seat_client != seat_client) return;
    if (!server->output.scene_output) return;

    wlr_cursor_set_surface(
        server->cursor,
        event->surface,
        event->hotspot_x,
        event->hotspot_y
    );
}

static void server_new_input(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, backend_new_input
    );
    struct wlr_input_device *device = data;

    if (device->type == WLR_INPUT_DEVICE_POINTER ||
        device->type == WLR_INPUT_DEVICE_TOUCH ||
        device->type == WLR_INPUT_DEVICE_TABLET)
    {
        wlr_cursor_attach_input_device(server->cursor, device);
    }

    if (device->type != WLR_INPUT_DEVICE_KEYBOARD)
        return;

    struct wlr_keyboard *keyboard =
        wlr_keyboard_from_input_device(device);

    struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(
        context, NULL, XKB_KEYMAP_COMPILE_NO_FLAGS
    );

    xkb_context_unref(context);

    wlr_keyboard_set_keymap(keyboard, keymap);
    xkb_keymap_unref(keymap);

    wlr_keyboard_set_repeat_info(keyboard, 25, 600);

    server->kb.keyboard = keyboard;
    server->kb.modifiers.notify = keyboard_handleModifiers;
    wl_signal_add(&keyboard->events.modifiers,
        &server->kb.modifiers);

    server->kb.key.notify = keyboard_handleKey;
    wl_signal_add(&keyboard->events.key, &server->kb.key);

    server->kb.destroy.notify = keyboard_destroy;
    wl_signal_add(&keyboard->base.events.destroy,
        &server->kb.destroy);

    wlr_seat_set_keyboard(server->seat, keyboard);
}

static void server_new_output(
    struct wl_listener *listener, void *data)
{
    Server *server = wl_container_of(
        listener, server, backend_new_output
    );
    struct wlr_output *output = data;

    struct wlr_output_state state = { 0 };
    wlr_output_state_init(&state);
    wlr_output_state_set_enabled(&state, true);

    struct wlr_output_mode *mode = wlr_output_preferred_mode(output);
    if (mode)
        wlr_output_state_set_mode(&state, mode);

    if (!wlr_output_commit_state(output, &state))
    {
        wlr_output_state_finish(&state);
        return;
    }

    wlr_output_state_finish(&state);

    wlr_output_init_render(output, server->allocator, server->renderer);

    struct wlr_scene_output *scene_output =
        wlr_scene_output_create(server->scene, output);

    struct wlr_output_layout_output *layout_output =
        wlr_output_layout_add_auto(
            server->output_layout, output
    );

    wlr_scene_output_layout_add_output(
        server->scene_layout, layout_output, scene_output
    );

    server->output.scene_output = scene_output;
    server->output.width = output->width;
    server->output.height = output->height;

    wlr_xcursor_manager_load(server->cursor_mgr, output->scale);

    fprintf(stderr, "Output: %s %ux%u\n",
        output->name, output->width, output->height);
}

static void process_cursor(Server *server)
{
    double sx, sy;
    struct wlr_scene_node *node =
        wlr_scene_node_at(
            &server->scene->tree.node,
            server->cursor->x,
            server->cursor->y,
            &sx, &sy
    );

    if (!node)
    {
        wlr_cursor_set_xcursor(
            server->cursor,
            server->cursor_mgr,
            "default"
        );
        wlr_seat_pointer_notify_clear_focus(server->seat);
        return;
    }

    if (node->type == WLR_SCENE_NODE_BUFFER)
    {
        struct wlr_scene_buffer *scene_buffer =
            wlr_scene_buffer_from_node(node);
        struct wlr_scene_surface *scene_surface =
            wlr_scene_surface_try_from_buffer(scene_buffer);

        if (scene_surface)
        {
            wlr_cursor_set_xcursor(
                server->cursor,
                server->cursor_mgr,
                "default"
            );
            wlr_seat_pointer_notify_enter(
                server->seat,
                scene_surface->surface,
                sx, sy
            );
            wlr_seat_pointer_notify_motion(
                server->seat,
                0,
                sx, sy
            );
            return;
        }
    }

    wlr_cursor_set_xcursor(
        server->cursor,
        server->cursor_mgr,
        "default"
    );
    wlr_seat_pointer_notify_clear_focus(server->seat);
}

void input_init(Server *server)
{
    server->cursor = wlr_cursor_create();
    wlr_cursor_attach_output_layout(server->cursor, server->output_layout);

    server->cursor_mgr = wlr_xcursor_manager_create(NULL, 24);

    server->backend_new_input.notify = server_new_input;
    wl_signal_add(&server->backend->events.new_input,
        &server->backend_new_input);

    server->backend_new_output.notify = server_new_output;
    wl_signal_add(&server->backend->events.new_output,
        &server->backend_new_output);

    server->seat_listeners.request_set_cursor.notify =
        seat_request_set_cursor;
    wl_signal_add(
        &server->seat->events.request_set_cursor,
        &server->seat_listeners.request_set_cursor
    );
}

void input_run(Server *server)
{
    process_cursor(server);
}

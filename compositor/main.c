#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "server.h"

static Server server;

static void signal_handler(int sig)
{
    (void)sig;
    wl_display_terminate(server.display);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    struct sigaction sa =
    {
        .sa_handler = signal_handler,
    };
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    server_init(&server);
    server_run(&server);
    server_destroy(&server);

    return 0;
}

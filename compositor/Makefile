CC = gcc
CFLAGS = -g -O0 -Wall -Wextra -Wpedantic -std=c11 \
	 $(shell pkg-config --cflags wlroots-0.20 wayland-server) \
	 $(shell pkg-config --cflags cairo pangocairo egl glesv2 xkbcommon) \
	 -DWLR_USE_UNSTABLE
LDFLAGS = $(shell pkg-config --libs wlroots-0.20 wayland-server) \
	  $(shell pkg-config --libs cairo pangocairo egl glesv2 xkbcommon)

SOURCES = main.c server.c xdg_shell.c input.c ui.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = desktop

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean

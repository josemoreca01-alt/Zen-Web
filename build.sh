#!/bin/sh

# Compila Zen usando pkg-config para detectar WebKitGTK y GTK
gcc src/zen.c -o zen $(pkg-config --cflags --libs webkit2gtk-4.0 gtk+-3.0)
gcc src/zen-kiosk.c -o zen-kiosk $(pkg-config --cflags --libs webkit2gtk-4.0 gtk+-3.0)
gcc src/zen-web.c -o zen-web `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-4.0`

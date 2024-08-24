#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

int is_window_fullscreen(Display *display, Window window) {
    Atom net_wm_state = XInternAtom(display, "_NET_WM_STATE", False);
    Atom net_wm_state_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", False);

    Atom actual_type;
    int actual_format;
    unsigned long num_items, bytes_after;
    unsigned char *prop = NULL;

    int fullscreen = 0;
    if (XGetWindowProperty(display, window, net_wm_state, 0, (~0L), False, XA_ATOM,
                           &actual_type, &actual_format, &num_items, &bytes_after, &prop) == Success) {
        if (prop) {
            Atom *atoms = (Atom *)prop;
            for (unsigned long i = 0; i < num_items; i++) {
                if (atoms[i] == net_wm_state_fullscreen) {
                    fullscreen = 1;
                    break;
                }
            }
            XFree(prop);
        }
    }
    return fullscreen;
}

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        return 1;
    }

    Window root = DefaultRootWindow(display);
    Window focused_window;
    int revert;

    XGetInputFocus(display, &focused_window, &revert);

    int fullscreen = is_window_fullscreen(display, focused_window);

    XCloseDisplay(display);
    return fullscreen ? 0 : 1;
}


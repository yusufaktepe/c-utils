#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

int main() {
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Failed to open X display\n");
        return 1;
    }

    Window root = DefaultRootWindow(display);
    Window focused;
    int revert;
    XGetInputFocus(display, &focused, &revert);

    if (focused == None || focused == PointerRoot) {
        fprintf(stderr, "No focused window found\n");
        XCloseDisplay(display);
        return 1;
    }

    Atom wm_class = XInternAtom(display, "WM_CLASS", True);
    if (wm_class == None) {
        fprintf(stderr, "WM_CLASS atom not found\n");
        XCloseDisplay(display);
        return 1;
    }

    Atom actual_type;
    int actual_format;
    unsigned long nitems, bytes_after;
    unsigned char *prop = NULL;

    if (XGetWindowProperty(display, focused, wm_class, 0, 1024, False, XA_STRING,
                            &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success && prop) {
        char *instance = (char *)prop;
        char *class = instance + strlen(instance) + 1;
        printf("WM_CLASS=%s\n", class);
        printf("WM_INSTANCE=%s\n", instance);
        XFree(prop);
    } else {
        fprintf(stderr, "Failed to get WM_CLASS property\n");
    }

    XCloseDisplay(display);
    return 0;
}


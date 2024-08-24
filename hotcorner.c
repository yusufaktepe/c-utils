#define _POSIX_C_SOURCE 199309L
#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <getopt.h>

#define DEFAULT_CHECK_INTERVAL 50
#define DEFAULT_TRIGGER_TIME 50

typedef enum {
    TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT, NONE
} Corner;

void
usage(const char *prog_name)
{
    printf("Usage: %s [OPTIONS]\n\n", prog_name);
    printf("Options:\n");
    printf("  --check-interval ms   Interval in ms between checks (default: %d)\n",
           DEFAULT_CHECK_INTERVAL);
    printf("  --trigger-time ms     Time in ms before action triggers (default: %d)\n",
           DEFAULT_TRIGGER_TIME);
    printf("  --top-left cmd        Command for top-left corner\n");
    printf("  --bottom-left cmd     Command for bottom-left corner\n");
    printf("  --top-right cmd       Command for top-right corner\n");
    printf("  --bottom-right cmd    Command for bottom-right corner\n");
    printf("  --help                Show this message\n\n");
    exit(EXIT_SUCCESS);
}

void
run_action(const char *command)
{
    if (command)
        system(command);
}

int
main(int argc, char *argv[])
{
    if (argc == 1)
        usage(argv[0]);

    int check_interval = DEFAULT_CHECK_INTERVAL;
    int corner_stay_time = DEFAULT_TRIGGER_TIME * 1000000L;
    char *commands[4] = {NULL};
    struct option long_options[] = {
        {"check-interval", required_argument, 0, 'c'},
        {"trigger-time",   required_argument, 0, 't'},
        {"top-left",       required_argument, 0, '1'},
        {"bottom-left",    required_argument, 0, '2'},
        {"top-right",      required_argument, 0, '3'},
        {"bottom-right",   required_argument, 0, '4'},
        {"help",           no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };
    int opt, option_index;
    while ((opt = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
        if (opt == 'h' || opt == '?')
            usage(argv[0]);
        if (!optarg || (optind > argc || argv[optind - 1][0] == '-')) {
            fprintf(stderr, "Error: Option '--%s' requires a value.\n", long_options[option_index].name);
            exit(EXIT_FAILURE);
        }
        if (opt == 'c')
            check_interval = atoi(optarg);
        else if (opt == 't')
            corner_stay_time = atoi(optarg) * 1000000L;
        else if (opt >= '1' && opt <= '4')
            commands[opt - '1'] = optarg;
        else
            usage(argv[0]);
    }
    if (optind < argc)
        usage(argv[0]);

    Display *d;
    if (!(d = XOpenDisplay(NULL))) {
        fprintf(stderr, "Failed to open X display\n");
        return 1;
    }
    int scr = DefaultScreen(d);
    int w = DisplayWidth(d, scr), h = DisplayHeight(d, scr);
    Window root = RootWindow(d, scr);
    struct timespec req = {check_interval / 1000, (check_interval % 1000) * 1000000L};
    struct timespec start, current;
    Corner in_corner = NONE;
    int triggered = 0, x, y;
    for (;;) {
        nanosleep(&req, NULL);
        Window dummy;
        int dx, dy;
        unsigned int mask;
        XQueryPointer(d, root, &dummy, &dummy, &x, &y, &dx, &dy, &mask);
        Corner corner = (x == 0 && y == 0) ? TOP_LEFT :
                        (x == 0 && y == h - 1) ? BOTTOM_LEFT :
                        (x == w - 1 && y == 0) ? TOP_RIGHT :
                        (x == w - 1 && y == h - 1) ? BOTTOM_RIGHT : NONE;
        if (corner != NONE) {
            if (in_corner == NONE) {
                clock_gettime(CLOCK_MONOTONIC, &start);
                in_corner = corner;
                triggered = 0;
            } else if (!triggered) {
                clock_gettime(CLOCK_MONOTONIC, &current);
                long elapsed = (current.tv_sec - start.tv_sec) * 1000000000L +
                               (current.tv_nsec - start.tv_nsec);
                if (elapsed >= corner_stay_time) {
                    run_action(commands[corner]);
                    triggered = 1;
                }
            }
        } else {
            in_corner = NONE;
            triggered = 0;
        }
    }
    XCloseDisplay(d);
    return 0;
}


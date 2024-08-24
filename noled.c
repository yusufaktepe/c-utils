#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEFAULT_KEYBOARD_DEVICE "/dev/input/event5"  // Default device

void set_led(int fd, int led, int value) {
    struct input_event ie;
    ie.type = EV_LED;
    ie.code = led;
    ie.value = value;
    write(fd, &ie, sizeof(ie));
}

int main(int argc, char *argv[]) {
    const char *keyboard_device = DEFAULT_KEYBOARD_DEVICE;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            keyboard_device = argv[i + 1];
        }
    }

    int fd = open(keyboard_device, O_WRONLY);
    if (fd == -1) {
        perror("Error opening device");
        return 1;
    }

    /* Need to turn it on first—tp weirdness */
    set_led(fd, LED_CAPSL, 1); // Turn on Caps Lock LED
    set_led(fd, LED_CAPSL, 0); // Turn off Caps Lock LED

    close(fd);
    return 0;
}


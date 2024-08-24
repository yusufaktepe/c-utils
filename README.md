# Misc small utils, written in C

**Table of Contents**

- [net_monitor](#net_monitor)
- [noled](#noled)
- [check_fullscreen (X11)](#check_fullscreen-x11)
- [hotcorner (X11)](#hotcorner-x11)
- [wmclass (X11)](#wmclass-x11)
- [unique](#unique)

## net_monitor

Monitors internet connection and notifies when it's down/up.

Build:

```sh
gcc -o net_monitor net_monitor.c
```

## noled

Turns off CapsLock LED. I'm using it to immediately turn off annoying LED whenever it's tapped/hold. (*see [here.](https://github.com/yusufaktepe/dotfiles/blob/tp/.config/kanata/kanata.kbd)*)

Build:

```sh
gcc -o noled noled.c
```

## check_fullscreen (X11)

Checks if the current window is full screen and returns true/false.

Build:

```sh
gcc -o check_fullscreen check_fullscreen.c -lX11
```

## hotcorner (X11)

Runs a command when the mouse hits the screen corner.

Build:

```sh
gcc -o hotcorner hotcorner.c -lX11 -lXtst
```

Usage:

```sh
./hotcorner --top-left "notify-send 'topleft'" --top-right "notify-send 'topright'"
```

## wmclass (X11)

Prints out window class/instance in the following format:

```sh
WM_CLASS=CLASS
WM_INSTANCE=INSTANCE
```

Build:

```sh
gcc -o wmclass wmclass.c -lX11
```

## unique

Reads from stdin and outputs unique lines without sorting. (*or* just use: `awk -v OFS="\n" '!x[$0]++'`)

- Requires `uthash`.

Build:

```sh
gcc -o unique unique.c
```


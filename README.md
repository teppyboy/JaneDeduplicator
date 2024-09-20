# JaneDeduplicator

Deduplicate mouse clicks in Windows to fix buggy mouses (I'm pointing at you, Logitech G203)

## Usage

Run the program executable

## Building

Make sure you have `make` and `cmake` installed.

```
cmake .
make
```

Then there'll be a program named `JaneDeduplicator.exe` in the root of the project directory.

## FAQ

### Linux?

I'm too skill issue to write code using Linux APIs and the codebase is entirely based on Win32 APIs.

### It's still double clicking when the cursor is in the taskbar

Because this program "disables" the window and "enables" the window quickly to prevent double clicking and the taskbar is not a window, it doesn't work.

### It doesn't work, etc...

Try setting `--disable-press-time=X` where X is the time in ms to something higher than default (17)

Also, you should've noticed the message in the console:
```
!!! EARLY IMPLEMENTATION MAY NOT WORK AT ALL !!!
```
:)

### Can't click anymore :(

Try setting `--wait-before-disable=X` where X is the time in ms to something higher than default (10)

## License

[MIT](./LICENSE)
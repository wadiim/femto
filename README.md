# Femto

Femto is a cross-platform, command-line text editor.

## Building

``` shell
cmake . -Bbuild
cmake --build build
```

## Usage

``` shell
femto [file]
```

## Key bindings

| Keystroke | Effect |
| ------------- | -------- |
| <kbd>Ctrl</kbd>+<kbd>Q</kbd> | Quit |
| <kbd>Ctrl</kbd>+<kbd>S</kbd> | Save |
| <kbd>Home</kbd> | Move to the first character of the line |
| <kbd>End</kbd> | Move to the end of the line |
| <kbd>Page up</kbd> | Scroll up one page |
| <kbd>Page down</kbd> | Scroll down one page |
| <kbd>Ctrl</kbd>+<kbd>←</kbd> | Move backward to the beginning of a word |
| <kbd>Ctrl</kbd>+<kbd>→</kbd> | Move forward to the beginning of a word |
| <kbd>Ctrl</kbd>+<kbd>Home</kbd> | Jump to the beginning of the file |
| <kbd>Ctrl</kbd>+<kbd>End</kbd> | Jump to the end of the file |

## License

[MIT](https://github.com/wadiim/femto/blob/master/LICENSE)

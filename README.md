# Wave Menu

A dynamic terminal banner visualizer written in C using `ncurses`. It creates a scrolling "wave" of color over ASCII art text, supporting both file-based banners and real-time generation via `figlet`.

## Features

*   **Dynamic Wave Effect**: A customizable color wave that moves diagonally across your ASCII banner.
*   **Multi-color Support**: Define multiple colors for the wave to create complex patterns.
*   **Rainbow Mode**: A built-in 6-color rainbow cycle for maximum aesthetic.
*   **Flexible Input**:
    *   Load ASCII art from a local file.
    *   Generate text on the fly using `figlet`.
*   **Wide Character Support**: Uses `ncursesw` and `locale.h` to handle banners with special characters or UTF-8.
*   **Responsive**: Centered display that adjusts to your terminal size.

## Prerequisites

To compile and run `wave_menu`, you need:

1.  **ncurses**: Development libraries for terminal UI.
2.  **figlet** (optional): Required only if you use the `-t` or `--text` option.

### Installation

#### Arch Linux (btw)
```bash
sudo pacman -S ncurses figlet
```

#### Debian/Ubuntu
```bash
sudo apt update
sudo apt install libncursesw5-dev figlet
```

## Compilation

Compile the source using `gcc`. Ensure you link the wide-character version of ncurses (`-lncursesw`) and the math library (`-lm`):

```bash
gcc wave_menu.c -o wave_menu -lncursesw -lm
```

## Usage

```bash
./wave_menu [options]
```

### Options

| Option | Long Option | Description |
| :--- | :--- | :--- |
| `-h` | `--help` | Show the help message. |
| `-f <path>` | `--from-file` | Load a banner from a specific file (default: `banner`). |
| `-t <string>`| `--text` | Generate a banner from a string using `figlet`. |
| `-c <c1,c2>` | `--color` | Set custom wave colors (e.g., `red,yellow,cyan`). |
| `-b` | `--bold` | Enable bold mode for the entire text. |
| | `--rainbow` | Enable the 6-color rainbow wave. |

### Available Colors
`red`, `green`, `blue`, `yellow`, `magenta`, `cyan`, `white`.

## Examples

**1. Display a banner from a file with a cyan wave:**
```bash
./wave_menu -f my_banner.txt -c cyan
```

**2. Generate a "HELLO" banner with a rainbow effect:**
```bash
./wave_menu -t "HELLO" --rainbow
```

**3. Use multiple custom colors with bold text:**
```bash
./wave_menu -t "WAVE" -c red,blue,white -b
```

## Controls

*   **Q / q**: Exit the application.

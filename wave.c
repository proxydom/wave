#define _DEFAULT_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED 1
#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <getopt.h>

#define BASE_WAVE_WIDTH 20
#define MAX_COLORS 10

// Struttura per la configurazione
typedef struct {
    char *text;
    char *file_path;
    int colors[MAX_COLORS];
    int num_colors;
    int rainbow_mode;
    int bold_mode;
} Config;

// Prototipi
void wave_effect(const char *text, int len, Config *config);
void show_help(const char *prog_name);
int parse_color(const char *color_str);

void show_help(const char *prog_name) {
    printf("Usage: %s [options]\n", prog_name);
    printf("Options:\n");
    printf("  -h, --help                 Show this help message.\n");
    printf("  -f, --from-file <path>     Load banner from a file.\n");
    printf("  -t, --text <string>        Generate banner text with figlet.\n");
    printf("  -c, --color <c1,c2,...>    Set wave colors (e.g., red,yellow). Available: red, green, blue, yellow, magenta, cyan, white.\n");
    printf("  -b, --bold                 Enable bold for the entire text.\n");
    printf("      --rainbow              Enable rainbow mode with multiple waves.\n");
    exit(0);
}

int parse_color(const char *color_str) {
    if (strcasecmp(color_str, "red") == 0) return COLOR_RED;
    if (strcasecmp(color_str, "green") == 0) return COLOR_GREEN;
    if (strcasecmp(color_str, "blue") == 0) return COLOR_BLUE;
    if (strcasecmp(color_str, "yellow") == 0) return COLOR_YELLOW;
    if (strcasecmp(color_str, "magenta") == 0) return COLOR_MAGENTA;
    if (strcasecmp(color_str, "cyan") == 0) return COLOR_CYAN;
    if (strcasecmp(color_str, "white") == 0) return COLOR_WHITE;
    return -1; // Colore non valido
}

void wave_effect(const char *text, int len, Config *config) {
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    curs_set(0);
    noecho();
    nodelay(stdscr, TRUE);

    attr_t default_attr = config->bold_mode ? A_BOLD : A_NORMAL;

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    for (int i = 0; i < config->num_colors; i++) {
        init_pair(i + 2, config->colors[i], COLOR_BLACK);
    }

    int num_rows = 1;
    int max_banner_width = 0;
    int current_line_width = 0;
    int i = 0;
    while (i < len) {
        if (text[i] == '\n') {
            num_rows++;
            if (current_line_width > max_banner_width) max_banner_width = current_line_width;
            current_line_width = 0;
            i++;
            continue;
        }
        wchar_t wc;
        int bytes = mbtowc(&wc, &text[i], len - i);
        if (bytes <= 0) { i++; continue; }
        int width = wcwidth(wc);
        if (width > 0) current_line_width += width;
        i += bytes;
    }
    if (current_line_width > max_banner_width) max_banner_width = current_line_width;

    double wave_pos[MAX_COLORS];
    int first_run = 1;
    float wave_width = (config->num_colors > 0) ? (float)BASE_WAVE_WIDTH / config->num_colors : 0;
    float wave_separation = (config->num_colors > 1) ? (max_banner_width * 1.5) / config->num_colors : 0;

    while(1) {
        int max_row, max_col;
        getmaxyx(stdscr, max_row, max_col);
        (void)max_row;
        int initial_start_col = (max_col > max_banner_width) ? (max_col - max_banner_width) / 2 : 0;

        if (first_run) {
            for(int j = 0; j < config->num_colors; j++) {
                wave_pos[j] = initial_start_col - wave_width + (j * wave_separation);
            }
            first_run = 0;
        }

        clear();

        int row = 0;
        int col = initial_start_col;
        i = 0;
        while (i < len) {
            if (text[i] == '\n') {
                row++;
                col = initial_start_col;
                i++;
                continue;
            }

            wchar_t wc;
            int bytes = mbtowc(&wc, &text[i], len - i);
            if (bytes <= 0) { i++; continue; }
            
            int char_width = wcwidth(wc);
            if (char_width < 0) char_width = 1;

            double diagonal_pos = col + row * 0.5;
            
            cchar_t cc;
            setcchar(&cc, &wc, 0, 0, NULL);

            move(row, col);
            
            int colored = 0;
            for (int j = 0; j < config->num_colors; j++) {
                if (fabs(diagonal_pos - wave_pos[j]) < wave_width / 2.0) {
                    wattr_on(stdscr, COLOR_PAIR(j + 2) | A_BOLD, NULL);
                    add_wch(&cc);
                    wattr_off(stdscr, COLOR_PAIR(j + 2) | A_BOLD, NULL);
                    colored = 1;
                    break;
                }
            }

            if (!colored) {
                wattr_on(stdscr, COLOR_PAIR(1) | default_attr, NULL);
                add_wch(&cc);
                wattr_off(stdscr, COLOR_PAIR(1) | default_attr, NULL);
            }
            
            col += char_width;
            i += bytes;
        }

        refresh();
        usleep(40000);

        int ch = getch();
        if (ch != ERR) {
            if (ch == 'q' || ch == 'Q') break;
        }

        double max_screen_pos = initial_start_col + max_banner_width + num_rows * 1.5;
        for(int j = 0; j < config->num_colors; j++) {
            wave_pos[j] += 1.0;
            if (wave_pos[j] > max_screen_pos + wave_width) {
                wave_pos[j] = initial_start_col - wave_width - wave_separation;
            }
        }
    }

    endwin();
}

int main(int argc, char *argv[]) {
    Config config = {0};
    config.file_path = "banner"; // Default file

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"from-file", required_argument, 0, 'f'},
        {"text", required_argument, 0, 't'},
        {"color", required_argument, 0, 'c'},
        {"rainbow", no_argument, 0, 'r'},
        {"bold", no_argument, 0, 'b'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hf:t:c:b", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h': show_help(argv[0]); break;
            case 'f': config.file_path = optarg; break;
            case 't': config.text = optarg; break;
            case 'c': {
                char *token = strtok(optarg, ",");
                while (token != NULL && config.num_colors < MAX_COLORS) {
                    int color = parse_color(token);
                    if (color != -1) {
                        config.colors[config.num_colors++] = color;
                    }
                    token = strtok(NULL, ",");
                }
                break;
            }
            case 'r': config.rainbow_mode = 1; break;
            case 'b': config.bold_mode = 1; break;
            default: exit(1);
        }
    }

    if (config.rainbow_mode) {
        config.num_colors = 6;
        config.colors[0] = COLOR_RED;
        config.colors[1] = COLOR_YELLOW;
        config.colors[2] = COLOR_GREEN;
        config.colors[3] = COLOR_CYAN;
        config.colors[4] = COLOR_BLUE;
        config.colors[5] = COLOR_MAGENTA;
    } else if (config.num_colors == 0) {
        config.num_colors = 1;
        config.colors[0] = COLOR_RED;
    }

    char *buffer = (char*)calloc(800000, sizeof(char));
    if (!buffer) { perror("calloc"); return 1; }
    size_t len = 0;

    if (config.text) {
        char command[512];
        snprintf(command, sizeof(command), "figlet -w 200 '%s'", config.text);
        FILE *fp = popen(command, "r");
        if (fp == NULL) { perror("popen figlet"); free(buffer); return 1; }
        len = fread(buffer, 1, 799999, fp);
        pclose(fp);
    } else {
        FILE *input = fopen(config.file_path, "r");
        if (input == NULL) { perror("fopen"); free(buffer); return 1; }
        len = fread(buffer, 1, 799999, input);
        fclose(input);
    }

    if (len == 0) {
        fprintf(stderr, "Error: No text to display. Banner file might be empty or figlet failed.\n");
        free(buffer);
        return 1;
    }
    
    char* p = buffer;
    char* writer = buffer;
    for (size_t i = 0; i < len; i++) {
        if (p[i] != '\r') {
            *writer++ = p[i];
        }
    }
    *writer = '\0';
    len = writer - buffer;

    wave_effect(buffer, len, &config);

    free(buffer);
    return 0;
}
/*
 * Copyright (c) 2025 Ashkan Feyzollahi
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is source for MeteorMayhem program.
 *
 * Author:          Ashkan Feyzollahi <ashkanfeyzollahi@gmail.com>
 */

#include <argp.h>
#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define MAX(x, y)                 ((x) > (y) ? (x) : (y))
#define MIN(x, y)                 ((x) < (y) ? (x) : (y))

#define NF_HEART                  "\xf3\xb0\x8b\x91"
#define NF_METEOR                 "\xee\xbc\xa3 "
#define NF_RAY                    "\xe2\x8f\xbd "
#define NF_RAYS                   "\xf3\xb1\x90\x8b"
#define NF_ROCKET                 "\xf3\xb0\x91\xa3 "
#define ASCII_HEART               "@"
#define ASCII_METEOR              "*"
#define ASCII_RAY                 "|"
#define ASCII_RAYS                "!"
#define ASCII_ROCKET              "^"

#define ASCII_STATISTICS_FORMAT   " health: %d > rays: %d > score: %d "
#define NF_STATISTICS_FORMAT      " \xf3\xb0\x8b\x91 %d \xee\xaa\xb6 \xf3\xb1\x90\x8b %d \xee\xaa\xb6 \xf3\xb1\x89\xbe  %d "


const char * argp_program_bug_address = "ashkanfeyzollahi@gmail.com";
const char * argp_program_version = "version 0.1.0";

static int
parse_opt(int key, char * arg,
      struct argp_state * state) {
    char * flags = state->input;

    switch (key) {
        case 'n':
            *flags = (*flags) | 1;
            break;

        case 's':
            if (arg == NULL || strcmp(arg, "visible") == 0) {
                *flags |= 2;
            } else if (strcmp(arg, "invisible") == 0) {
                if ((*flags) & 2) {
                    *flags ^= 2;
                }
            } else {
                argp_failure(state, 1, 0, "invalid argument `%s` for `-s` or `--statistics`\n"
                    "valid arguments are:\n`visible`, `invisible`", arg);
            }

            break;

        case ARGP_KEY_ARG:
            argp_failure(state, 1, 0, "too many arguments");
            break;
    }
    return 0;
}

int
main(int argc, char *argv[]) {
    struct argp_option options[] = {
        { 0,            0,   0,           0,                   "User Interface Options:", 1 },
        { "nerdfont",   'n', 0,           0,                   "Prefer using NerdFont with utf-8 encoding" },
        { "statistics", 's', "VISIBLITY", OPTION_ARG_OPTIONAL, "Change player statistics' VISIBLITY" },
        { 0,             0,  0,           0,                   "Information Options:",    -1 },
        { 0 }
    };

    struct argp argp = { options, parse_opt };

    char flags = 2;

    argp_parse(&argp, argc, argv, 0, 0, &flags);

    if (flags & 1) {
        setlocale(LC_ALL, "");
    }

    char * health_sign, * meteor_sign, * player_sign, * ray_sign, * rays_sign, * statistics, * statistics_format;
    double clock_start[4];
    int health, key = 0, * meteor_array, old_screen_columns, player_x, player_y, player_score, rand_meteor, rays, screen_columns, screen_rows, sizeof_meteor_array, statistics_x_offset;

    clock_start[0] = clock();
    clock_start[1] = clock();
    clock_start[2] = 0;
    clock_start[3] = clock();
    health = 100;
    player_score = 0;
    rays = 5;

    if (flags & 1) {
        health_sign = NF_HEART;
        meteor_sign = NF_METEOR;
        player_sign = NF_ROCKET;
        ray_sign = NF_RAY;
        rays_sign = NF_RAYS;
        statistics_format = NF_STATISTICS_FORMAT;
        statistics_x_offset = 7;
    } else {
        health_sign = ASCII_HEART;
        meteor_sign = ASCII_METEOR;
        player_sign = ASCII_ROCKET;
        ray_sign = ASCII_RAY;
        rays_sign = ASCII_RAYS;
        statistics_format = ASCII_STATISTICS_FORMAT;
        statistics_x_offset = 0;
    }

    initscr();

    screen_rows = getmaxy(stdscr);
    screen_columns = getmaxx(stdscr);
    old_screen_columns = screen_columns;
    sizeof_meteor_array = sizeof(int) * screen_columns;
    meteor_array = (int *) malloc(sizeof_meteor_array);
    memset(meteor_array, -1, sizeof_meteor_array);

    player_x = getmaxx(stdscr) / 2;
    player_y = getmaxy(stdscr) / 2 - 1;

    if (has_colors()) {
        use_default_colors();
        start_color();
    }

    curs_set(0);
    cbreak();
    scrollok(stdscr, FALSE);
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    noecho();

    init_pair(1, COLOR_YELLOW, -1);
    init_pair(2, COLOR_RED, -1);
    init_pair(3, COLOR_MAGENTA, -1);
    init_pair(4, COLOR_CYAN, -1);

    while (key != 'q' && health > 0) {
        erase();

        screen_rows = getmaxy(stdscr);
        screen_columns = getmaxx(stdscr);

        if (sizeof_meteor_array != sizeof(int) * screen_columns) {
            sizeof_meteor_array = sizeof(int) * screen_columns;
            meteor_array = (int *) realloc(meteor_array, sizeof_meteor_array);
            if (old_screen_columns < screen_columns) {
                memset(&meteor_array[old_screen_columns], -1, sizeof(int) * (screen_columns - old_screen_columns));
            }
            old_screen_columns = screen_columns;
        }

        if (flags & 2) {
            --screen_rows;

            attron(COLOR_PAIR(3));

            statistics = (char *) malloc(screen_columns);
            sprintf(statistics, statistics_format, health, rays, player_score);

            move(screen_rows, screen_columns / 2 - strlen(statistics) / 2 + statistics_x_offset);
            addstr(statistics);

            free(statistics);

            attroff(COLOR_PAIR(3));
        }

        if ((clock() - clock_start[0]) / CLOCKS_PER_SEC >= 0.1) {
            for (int i = 0; i < screen_columns; i++) {
                if (meteor_array[i] <= -1) {
                    continue;
                }

                ++meteor_array[i];

                if (meteor_array[i] >= screen_rows) {
                    meteor_array[i] = -1;
                }
            }

            clock_start[0] = clock();
        }

        if ((clock() - clock_start[1]) / CLOCKS_PER_SEC >= 0.1) {
            clock_start[1] = clock();

            rand_meteor = rand() % screen_columns;

            if (meteor_array[rand_meteor] <= -1) {
                meteor_array[rand_meteor] = 0;
            }
        }

        for (int i = 0; i < screen_columns; i++) {
            if (meteor_array[i] <= -1) {
                continue;
            }

            attron(COLOR_PAIR(2));
            mvaddstr(meteor_array[i], i, meteor_sign);
            attroff(COLOR_PAIR(2));
        }

        player_y = MAX(MIN(player_y, screen_rows - 1), 0);
        player_x = MAX(MIN(player_x, screen_columns - 1), 0);

        attron(A_BOLD | COLOR_PAIR(1));
        mvaddstr(player_y, player_x, player_sign);
        attroff(A_BOLD | COLOR_PAIR(1));

        if (clock_start[2] != 0) {
            attron(COLOR_PAIR(4));
            for (int i = 0; i < player_y; i++) {
                mvaddstr(i, player_x, ray_sign);

                if (meteor_array[player_x] == i) {
                    meteor_array[player_x] = -1;
                    player_score += 5;
                }
            }
            attroff(COLOR_PAIR(4));

            if ((clock() - clock_start[2]) / CLOCKS_PER_SEC >= 0.1) {
                clock_start[2] = 0;
            }
        }

        if ((clock() - clock_start[3]) / CLOCKS_PER_SEC >= 30) {
            clock_start[3] = clock();
            health += 10;
            rays += 2;
        }

        if (meteor_array[player_x] == player_y) {
            health -= 20;
            meteor_array[player_x] = -1;
        }

        refresh();

        key = getch();

        if (clock_start[2] == 0) {
            switch (key) {
                case 'e':
                    if (rays > 0) {
                        clock_start[2] = clock();
                        --rays;
                    }
                    break;

                case KEY_UP:
                    player_y--;
                    break;

                case KEY_DOWN:
                    player_y++;
                    break;

                case KEY_LEFT:
                    player_x--;
                    break;

                case KEY_RIGHT:
                    player_x++;
                    break;

                case KEY_RESIZE:
                    old_screen_columns = screen_columns;
                    break;
            }
        }
    }

    endwin();
    free(meteor_array);
    printf("\n ~ Gameover! ~\n\n  * Player Score: %d\n\n", player_score);

    return EXIT_SUCCESS;
}

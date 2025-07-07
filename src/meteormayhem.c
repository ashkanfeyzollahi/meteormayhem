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


/* MACROS */
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


/* argp program bug address and version constants */
const char * argp_program_bug_address = "ashkanfeyzollahi@gmail.com";
const char * argp_program_version = "version 0.1.0";

/* parse_opt for parsing options */
static int
parse_opt(int key, char * arg,
      struct argp_state * state) {
    char * flags = state->input;

    switch (key) {
        /* nerdfont option */
        case 'n':
            *flags = (*flags) | 1;
            break;

        /* statistics option with an optional argument */
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

        /* fail if key == ARGP_KEY_ARG */
        case ARGP_KEY_ARG:
            argp_failure(state, 1, 0, "too many arguments");
            break;
    }
    return 0;
}

/* program main function/entry */
int
main(int argc, char *argv[]) {
    /* argp command line options */
    struct argp_option options[] = {
        { 0,            0,   0,           0,                   "User Interface Options:", 1 },
        { "nerdfont",   'n', 0,           0,                   "Prefer using NerdFont with utf-8 encoding" },
        { "statistics", 's', "VISIBLITY", OPTION_ARG_OPTIONAL, "Change player statistics' VISIBLITY" },
        { 0,             0,  0,           0,                   "Information Options:",    -1 },
        { 0 }
    };

    /* argp structure for command line parsing */
    struct argp argp = { options, parse_opt };

    /* program options/flags (char/1 byte) */
    char flags = 2;

    /* parse command line arguments using argp_parse */
    argp_parse(&argp, argc, argv, 0, 0, &flags);

    /* set locale to `LC_ALL` for ncurses utf-8 support if have to */
    if (flags & 1) {
        setlocale(LC_ALL, "");
    }

    /* declare variables */
    char * health_sign, * meteor_sign, * player_sign, * ray_sign, * rays_sign, * statistics, * statistics_format;
    double clock_start[4];
    int health, key, * meteor_array, old_screen_columns, player_x, player_y, player_score, rand_meteor, rays, screen_columns, screen_rows, sizeof_meteor_array, statistics_x_offset;

    /* initialize variables */
    clock_start[0] = clock();
    clock_start[1] = clock();
    clock_start[2] = 0;
    clock_start[3] = clock();
    health = 100;
    player_score = 0;
    rays = 5;

    /* prefer NerdFont UTF-8 characters if enabled else use ASCII characters */
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

    /* initialize ncurses stdscr */
    initscr();

    /* ncurses based variables' assignment */
    screen_rows = getmaxy(stdscr);
    screen_columns = getmaxx(stdscr);
    old_screen_columns = screen_columns;
    sizeof_meteor_array = sizeof(int) * screen_columns;
    meteor_array = (int *) malloc(sizeof_meteor_array);
    memset(meteor_array, -1, sizeof_meteor_array);

    /* move player to center of screen */
    player_x = getmaxx(stdscr) / 2;
    player_y = getmaxy(stdscr) / 2 - 1;

    /* setup ncurses */
    /* enable colored output if terminal emulator supports */
    if (has_colors()) {
        use_default_colors();
        start_color();
    }

    /* hide cursor */
    curs_set(0);

    /* enable cbreak mode */
    cbreak();

    /* disable scrolling for stdscr */
    scrollok(stdscr, FALSE);

    /* enable nodelay mode for stdscr */
    nodelay(stdscr, TRUE);

    /* enable keypad mode for stdscr */
    keypad(stdscr, TRUE);

    /* enable noecho mode */
    noecho();

    /* initialize ncurses color pairs */
    init_pair(1, COLOR_YELLOW, -1);
    init_pair(2, COLOR_RED, -1);
    init_pair(3, COLOR_MAGENTA, -1);
    init_pair(4, COLOR_CYAN, -1);

    /* while key != 'q' and health > 0 repeat steps inside loop block */
    while (key != 'q' && health > 0) {
        /* clear stdscr buffer */
        erase();

        /* get screen_columns & screen_rows */
        screen_rows = getmaxy(stdscr);
        screen_columns = getmaxx(stdscr);

        /* reallocate memory when screen_columns variable changes */
        if (sizeof_meteor_array != sizeof(int) * screen_columns) {
            sizeof_meteor_array = sizeof(int) * screen_columns;
            meteor_array = (int *) realloc(meteor_array, sizeof_meteor_array);

            if (old_screen_columns < screen_columns) {
                memset(&meteor_array[old_screen_columns + 1], -1, sizeof_meteor_array - sizeof(int) * old_screen_columns);
            }
        }

        /* print statistics if enabled */
        if (flags & 2) {
            /* decrease screen row */
            --screen_rows;

            /* enable attribute (color) */
            attron(COLOR_PAIR(3));

            /* allocate memory for statistics string */
            statistics = (char *) malloc(screen_columns);

            /* print formatted statistics_format to statistics */
            sprintf(statistics, statistics_format, health, rays, player_score);

            /* move to where to print statistics */
            move(screen_rows, screen_columns / 2 - strlen(statistics) / 2 + statistics_x_offset);

            /* print statistics */
            addstr(statistics);

            /* free allocated memory for statistics string */
            free(statistics);

            /* disable attribute (color) */
            attroff(COLOR_PAIR(3));
        }

        /* move meteors downward every 0.1 seconds */
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

        /* spawn a meteor every 0.1 seconds if possible */
        if ((clock() - clock_start[1]) / CLOCKS_PER_SEC >= 0.1) {
            clock_start[1] = clock();

            rand_meteor = rand() % screen_columns;

            if (meteor_array[rand_meteor] <= -1) {
                meteor_array[rand_meteor] = 0;
            }
        }

        /* print meteors to stdscr */
        for (int i = 0; i < screen_columns; i++) {
            if (meteor_array[i] <= -1) {
                continue;
            }

            attron(COLOR_PAIR(2));
            mvaddstr(meteor_array[i], i, meteor_sign);
            attroff(COLOR_PAIR(2));
        }

        /* calculate player coordinate */
        player_y = MAX(MIN(player_y, screen_rows - 1), 0);
        player_x = MAX(MIN(player_x, screen_columns - 1), 0);

        /* move to calculated player coordinate and print player */
        attron(A_BOLD | COLOR_PAIR(1));
        mvaddstr(player_y, player_x, player_sign);
        attroff(A_BOLD | COLOR_PAIR(1));

        /* print shot ray */
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

        /* give 10 health and 2 rays every 60 seconds */
        if ((clock() - clock_start[3]) / CLOCKS_PER_SEC >= 30) {
            clock_start[3] = clock();
            health += 10;
            rays += 2;
        }

        /* check if player had collisions */
        if (meteor_array[player_x] == player_y) {
            health -= 20;
            meteor_array[player_x] = -1;
        }

        /* refresh and print everything on stdscr to real screen */
        refresh();

        /* check if any key is being pressed */
        key = getch();

        /* allow player to move */
        if (clock_start[2] == 0) {
            switch (key) {
                case 'e':
                    /* set clock_start[2] to clock() (for shooting rays, cooldown etc.) */
                    if (rays > 0) {
                        clock_start[2] = clock();
                        --rays;
                    }
                    break;

                case KEY_UP:
                    /* move player to up */
                    player_y--;
                    break;

                case KEY_DOWN:
                    /* move player to down */
                    player_y++;
                    break;

                case KEY_LEFT:
                    /* move player to left */
                    player_x--;
                    break;

                case KEY_RIGHT:
                    /* move player to right */
                    player_x++;
                    break;

                case KEY_RESIZE:
                    /* assign old_screen_columns */
                    old_screen_columns = screen_columns;
                    break;
            }
        }
    }

    /* free all allocated memory for ncurses */
    endwin();

    /* print gameover message */
    printf("\n ~ Gameover! ~\n\n  * Player Score: %d\n\n", player_score);

    /* exit program with success code */
    return EXIT_SUCCESS;
}


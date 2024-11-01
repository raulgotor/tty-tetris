/*!
 *******************************************************************************
 * @file main.cpp
 *
 * @brief
 *
 * @authors Raúl Gotor (raulgotor@gmail.com)
 * @date 27.10.2024
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2024 Raúl Gotor
 * All rights reserved.
 *******************************************************************************
 */

/*
 *******************************************************************************
 * include Statements                                                         *
 *******************************************************************************
 */
#define _XOPEN_SOURCE_EXTENDED

#include <iostream>
#include <print>
#include <libc.h>
#include <ncurses.h>
#include <functional>

#include "Tetromino.h"

#include "Tetris.h"

/*
 *******************************************************************************
 * Private Macros                                                              *
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Data types                                                                  *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Constants                                                                   *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

static void drawPixel(int x, int y);

static int64_t get_microseconds(void);

static WINDOW * create_newwin(int height, int width, int starty, int startx, bool box);

/*
 *******************************************************************************
 * Public Data Declarations                                                    *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */

WINDOW * p_window_game;
WINDOW * p_window_frame;
WINDOW * p_window_next;

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

static void drawPixel(int x, int y)
{
        cchar_t wc_block;
        wchar_t block_char = L'\u2593';

        wmove(p_window_game, x, y);
        setcchar(&wc_block, &block_char, 0, 0, NULL);
        wadd_wch(p_window_game, &wc_block);
}

static int64_t get_microseconds(void) {
        struct timeval tv;

        gettimeofday(&tv, NULL);

        return (int64_t)(tv.tv_sec) * 1000 + (int64_t)(tv.tv_usec / 1000);
}

static WINDOW * create_newwin(int height, int width, int starty, int startx, bool _box)
{
        WINDOW * local_win;

        local_win = newwin(height, width, starty, startx);

        if (_box) {
                box(local_win, 0, 0);
        }

        wrefresh(local_win);

        return local_win;
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

int main()
{
        Tetris tetris(drawPixel, get_microseconds);
        int64_t last_update = 0;
        bool should_quit = false;

        int startx, starty, width, height;
        int ch;
        int64_t current_time;

        height = 20;
        width = 10;
        starty = 0;
        startx = 0;

        setlocale(LC_ALL, "");  // Use "" to set to the current locale, usually UTF-8
        initscr();
        cbreak();
        noecho();
        curs_set(0);

        refresh();
        p_window_next = create_newwin(6, 6, starty, width + 5, true);
        p_window_frame = create_newwin(height +2 , width+ 2, starty, startx, true);
        p_window_game = create_newwin(height, width, starty + 1, startx + 1, false);
        char const str[20] = "Score:";

        keypad(p_window_game, TRUE);


        tetris.startGame();

        while (!should_quit) {
                nodelay(p_window_game, TRUE);
                ch = wgetch(p_window_game);
                nodelay(p_window_game, FALSE);

                switch (ch) {
                        case KEY_LEFT:
                                tetris.moveLeft();
                                break;
                        case KEY_RIGHT:
                                tetris.moveRight();
                                break;
                        case KEY_UP:
                                tetris.rotate();
                                break;
                        case KEY_DOWN:
                                tetris.moveDown();
                                break;
                        case 'q':
                                should_quit = true;
                                break;
                }

                current_time = get_microseconds();

                if (current_time - last_update > 10) {
                        last_update = current_time;
                        wclear(p_window_game);

                        tetris.process();
                        move(8,15);
                        snprintf(const_cast<char *>(str), sizeof(str), "Score: %d", tetris.getScore());
                        addstr(str);
                        refresh();
                        wrefresh(p_window_game);
                }
        }
        endwin();

        return 0;
}




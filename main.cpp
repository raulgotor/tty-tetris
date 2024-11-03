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

#include <clocale>
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

static int64_t getMicroseconds(void);

static WINDOW * createWindow(int height, int width, int starty, int startx, bool box);

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

WINDOW * pWindowGame;
WINDOW * pWindowGameFrame;
WINDOW * pWindowNext;
WINDOW * pWindowNextFrame;
WINDOW * pWindowKeys;
WINDOW * pWindowStats;

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

static void drawPixel(int const x, int const y, WINDOW * pWindow)
{
        cchar_t block;
        wchar_t blockChar = L'\u2593';

        wmove(pWindow, x, y);
        setcchar(&block, &blockChar, 0, 0, NULL);
        wadd_wch(pWindow, &block);
}

static void drawAtMainWindow(int const x, int const y)
{
        drawPixel(x, y, pWindowGame);
}

static void drawAtHelperWindow(int x, int y)
{
        drawPixel(x, y, pWindowNext);
}


static int64_t getMicroseconds(void) {
        struct timeval tv;

        gettimeofday(&tv, NULL);

        return (int64_t)(tv.tv_sec) * 1000 + (int64_t)(tv.tv_usec / 1000);
}

static WINDOW * createWindow(int height, int width, int starty, int startx, bool _box)
{
        WINDOW * local_win;

        local_win = newwin(height, width, starty, startx);

        if (_box) {
                box(local_win, 0, 0);
        }

        wrefresh(local_win);

        return local_win;
}

static void setupUI(void)
{
        int margin = 1;
        int windowNextX = 15;
        int windowNextWidth = 6;
        int windowNextHeight = 2;
        int windowKeysHeight = 9;
        int windowKeysWidth = 15;

        int windowStatsHeight = 5;
        int windowStatsWidth = windowKeysWidth;

        int windowGameStartX = 0;
        int windowsTopStartY = 0;

        int windowGameWidth = 10;
        int windowGameHeight = 20;

        setlocale(LC_ALL, "");
        initscr();
        cbreak();
        noecho();
        curs_set(0);

        pWindowNext = createWindow(windowNextHeight, windowNextWidth, windowsTopStartY + margin,
                                   windowNextX + margin, true);
        pWindowNextFrame = createWindow(windowNextHeight + margin * 2, windowNextWidth + margin * 2,
                                        windowsTopStartY, windowNextX, true);


        pWindowGameFrame = createWindow(windowGameHeight + margin * 2, windowGameWidth + margin * 2,
                                        windowsTopStartY, windowGameStartX, true);
        pWindowGame = createWindow(windowGameHeight, windowGameWidth, windowsTopStartY + margin,
                                   windowGameStartX + margin, false);
        pWindowKeys = createWindow(windowKeysHeight, windowKeysWidth, windowsTopStartY + 13,
                                   windowGameStartX + 15, true);
        pWindowStats = createWindow(windowStatsHeight, windowStatsWidth, windowsTopStartY + 5,
                                    windowGameStartX + 15, true);

        keypad(pWindowGame, TRUE);
        nodelay(pWindowGame, TRUE);
        timeout(0);

        wmove(pWindowKeys, 1, 2);
        waddstr(pWindowKeys, "← Move Left");
        wmove(pWindowKeys, 2, 2);
        waddstr(pWindowKeys, "→ Move Right");
        wmove(pWindowKeys, 3, 2);
        waddstr(pWindowKeys, "↑ Rotate");
        wmove(pWindowKeys, 4, 2);
        waddstr(pWindowKeys, "↓ Move Down");
        wmove(pWindowKeys, 5, 2);
        waddstr(pWindowKeys, "Space - Drop");
        wmove(pWindowKeys, 6, 2);
        waddstr(pWindowKeys, "q Quit");
        wmove(pWindowKeys, 7, 2);
        waddstr(pWindowKeys, "n New");

        wmove(pWindowGameFrame, 0, 3);
        waddstr(pWindowGameFrame, "TETRIS");

        wmove(pWindowStats, 0, 2);
        waddstr(pWindowStats, "Statistics");

        wmove(pWindowKeys, 0, 3);
        waddstr(pWindowKeys, "Keyboard");

        wmove(pWindowNextFrame, 0, 2);
        waddstr(pWindowNextFrame, "Next");

        wrefresh(pWindowStats);
        wrefresh(pWindowGameFrame);
        wrefresh(pWindowKeys);
        wrefresh(pWindowNextFrame);

}

static void refreshUI(Tetris &tetris)
{
        char const str[20] = "Score:";

        wclear(pWindowNext);
        wclear(pWindowGame);

        tetris.process();
        wmove(pWindowStats, 1, 1);
        snprintf(const_cast<char *>(str),
                 sizeof(str), "Score: %d",
                 tetris.getScore());

        waddstr(pWindowStats, str);
        wmove(pWindowStats, 2, 1);
        snprintf(const_cast<char *>(str),
                 sizeof(str), "Level: %d",
                 tetris.getLevel());

        waddstr(pWindowStats, str);
        wmove(pWindowStats, 3, 1);
        snprintf(const_cast<char *>(str),
                 sizeof(str), "Lines: %d",
                 tetris.getLines());

        waddstr(pWindowStats, str);

        wrefresh(pWindowStats);
        wrefresh(pWindowGame);
        wrefresh(pWindowNext);
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

int main()
{
        Tetris tetris(drawAtMainWindow, drawAtHelperWindow, getMicroseconds);
        int64_t lastUpdate = 0;
        bool shouldQuit = false;
        int64_t currentTime;
        int ch;

        setupUI();

        tetris.startGame();

        while (!shouldQuit) {
                ch = wgetch(pWindowGame);

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
                        case ' ':
                                tetris.drop();
                                break;
                        case 'n':
                                tetris.startGame();
                                break;
                        case 'q':
                                shouldQuit = true;
                                break;
                }

                currentTime = getMicroseconds();

                if ((ERR != ch) || (100 < currentTime - lastUpdate)) {
                        lastUpdate = currentTime;
                        refreshUI(tetris);
                }
        }

        endwin();

        return 0;
}

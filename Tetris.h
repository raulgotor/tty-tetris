/*!
 *******************************************************************************
 * @file Tetris.h
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

#ifndef TETRIS_H
#define TETRIS_H

//! @brief Purpose of the class module
class Tetris
{

/*
 *******************************************************************************
 * Public Data Types                                                           *
 *******************************************************************************
 */

public:
        typedef enum {
                TETRIS_SIDE_LEFT = 0,
                TETRIS_SIDE_RIGHT,
                TETRIS_SIDE_BOTTOM,
        } Side_t;

/*
 *******************************************************************************
 * Public Constants                                                            *
 *******************************************************************************
 */

public:


/*
 *******************************************************************************
 * Private Constants                                                           *
 *******************************************************************************
 */

private:
        static int const ROWS = 20;
        static int const COLUMNS = 10;
        int total_lines = 0;
        constexpr static int const deleted_lines_multiplier[5] =
        {
                0,
                100,
                300,
                500,
                800
        };
        double g_per_cell[15] = {
                0.01667,
                0.021017,
                0.026977,
                0.035256,
                0.04693,
                0.06361,
                0.0879,
                0.1236,
                0.1775,
                0.2598,
                0.388,
                0.59,
                0.92,
                1.46,
                2.36,
        };
/*
 *******************************************************************************
 * Public Static Data Declarations                                             *
 *******************************************************************************
 */

public:


/*
 *******************************************************************************
 * Private Static Data Declarations                                            *
 *******************************************************************************
 */

private:


/*
 *******************************************************************************
 * Public Data Member Declarations                                             *
 *******************************************************************************
 */

public:


/*
 *******************************************************************************
 * Private Data Member Declarations                                            *
 *******************************************************************************
 */

private:
        int score;
        bool isGameRunning = false;
        int grid[ROWS][COLUMNS] = {0};
        bool should_delete_rows[ROWS] = {false};
        int buffer_grid[ROWS][COLUMNS] = {0};
        int64_t last_time = 0;
        int  iteration = 0;
        int level = 0;
        Tetromino * currentPart{};
        Tetromino * nextPart{};
        std::function<void(int x, int y)> printer{};
        std::function<void(int x, int y)> next_printer{};
        std::function<int64_t (void)> get_us{};

/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

public:

        Tetris(std::function<void(int x, int y)>,
               std::function<void(int x, int y)>,
               std::function<int64_t(void)> get_us);

        void moveRight(void);

        void moveLeft(void);

        void moveDown(void);

        void drop(void);

        void addPart(void);

        void rotate();

        void process();

        int getScore();

        int getLines();

        int getLevel();

        void startGame();

        bool isRunning();

        bool rowsToDelete(void);


/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

private:
        void draw(void);

        bool isOverlap(Tetromino &tetromino);

        void clearBuffer(void);

        bool areCoordinatesInBounds(int x, int y);

        void printCurrentPart(void);

        void printGrid(void);

        bool isCurrentPartPartiallyOutsideBounds(void);

        void addScore(int deleted_lines);

        void step();

        void addPartToGrid(void);

        void printBufferGrid(void);

        void printNextPart(void);

        int getLinesToDelete(void);

        void deleteRowAndShiftDown(int i);

        bool isTouchingSide(Side_t const partSide);

        bool isTouchingGridContent(void);

        int findShift(Tetromino &tetromino);

        int testKicks(Tetromino &tetromino);

        void animateDeleteRows(void (Tetris::*onAnimationEnd)(void));

        void setRow(bool b, int i);

        void deleteFullRows();
};

#endif //TETRIS_H

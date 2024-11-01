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
        int buffer_grid[ROWS][COLUMNS] = {0};
        int64_t last_time = 0;
        Tetromino * currentPart{};
        std::function<void(int x, int y)> printer{};
        std::function<int64_t (void)> get_us{};

/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

public:

        Tetris(std::function<void(int x, int y)>, std::function<int64_t(void)> get_us);

        void moveRight(void);

        void moveLeft(void);

        void moveDown(void);

        void addPart(void);

        void rotate();

        void process();

        int getScore();

        void startGame();

        bool isRunning();

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

        void step();

        void addPartToGrid(void);

        void printBufferGrid(void);

        void deleteFullLines(void);

        void deleteRowAndShiftDown(int i);

        bool isTouchingSide(Side_t const partSide);

        int findShift(Tetromino &tetromino);

        int testKicks(Tetromino &tetromino);

};

#endif //TETRIS_H

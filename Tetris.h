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
        int totalLines = 0;
        constexpr static int const deletedLinesMultiplier[5] =
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
        bool shouldDeleteRows[ROWS] = {false};
        int bufferGrid[ROWS][COLUMNS] = {0};
        int64_t lastTime = 0;
        int  iteration = 0;
        int level = 0;
        Tetromino * currentPart{};
        Tetromino * nextPart{};
        std::function<void(int x, int y)> printer{};
        std::function<void(int x, int y)> next_printer{};
        std::function<int64_t (void)> getMs{};

/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

public:

        Tetris(std::function<void(int x, int y)>,
               std::function<void(int x, int y)>,
               std::function<int64_t(void)> _getMs);

        void drop(void);

        int getLevel() const;

        int getLines() const;

        int getScore() const;

        bool isRunning() const;

        void moveDown(void);

        void moveLeft(void);

        void moveRight(void);

        void process();

        void rotate();

        void startGame();



/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

private:
        void addPart(void);

        void addScore(int const numberOfLines);

        void step(void);

        // Game Buffers

        void addPartToBuffer(void);

        void addPartToGrid(void);

        void clearBuffer(void);

        void clearGrid(void);

        // Drawing

        void draw(void);

        void drawBuffer(void);

        void drawGrid(void);

        void drawNextPart(void);

        // Delete lines

        void animateLineDeletion(void (Tetris::*onAnimationEnd)(void));

        void deleteCompletedLines();

        void deleteLineAndShiftDown(int const lineToDelete);

        int getCompleteLines(void);

        bool isAnyLineToDelete(void);

        void setLineAtGrid(bool const b, int const i);

        // Geometry checks

        bool isOverlap(Tetromino &tetromino);

        bool isPartOOB(void);

        bool isPartTouchingGridContent(void);

        bool isPartTouchingSide(Side_t const partSide);

        bool isPointInBounds(int const x, int const y);

        int testPartKicks(Tetromino &tetromino);
};

#endif //TETRIS_H

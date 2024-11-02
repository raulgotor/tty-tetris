/*!
 *******************************************************************************
 * @file Tetris.cpp
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

#include <functional>
#include <ncurses.h>
#include "Shape.h"
#include "Tetromino.h"
#include "Tetris.h"

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
 * Private Macros                                                              *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Static Data Declarations                                                    *
 *******************************************************************************
 */


/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

Tetris::Tetris(std::function<void(int x, int y)> fprinter,
               std::function<void(int x, int y)> fnext_printer,
               std::function<int64_t(void)> _get_us)
{
        printer = fprinter;
        next_printer = fnext_printer;
        get_us = _get_us;
}

void Tetris::moveLeft(void)
{
        if (!isTouchingSide(TETRIS_SIDE_LEFT)) {
                currentPart->move(-1);
        }
}


void Tetris::moveRight(void)
{
        if (!isTouchingSide(TETRIS_SIDE_RIGHT)) {
                currentPart->move(1);
        }
}


static std::unique_ptr<Shape> getRandomShape(void)
{
        // Array of factory functions
        std::array<std::unique_ptr<Shape>(*)(), 7> shapeFactories = {
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<TShape>(); },
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<SShape>(); },
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<IShape>(); },
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<JShape>(); },
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<LShape>(); },
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<OShape>(); },
                +[]() -> std::unique_ptr<Shape> { return std::make_unique<ZShape>(); }
        };
        std::srand(std::time(nullptr));

        // Generate a random number between 1 and 10
        int random_number = (std::rand() % 6);

        // Create and use shapes
        return shapeFactories[random_number]();
}

void Tetris::addPart(void)
{
        this->currentPart = nextPart;
        this->nextPart = new Tetromino(std::move(getRandomShape()));
}


void Tetris::startGame()
{
        score = 0;
        level = 0;
        this->nextPart = new Tetromino(std::move(getRandomShape()));

        addPart();
        isGameRunning = true;
}


void Tetris::rotate()
{
        Tetromino testing_copy(*currentPart);
        int const (* p_available_kicks)[5][2];
        int const (* p_kick_op);
        int allowed_kick_idx;

        if (currentPart->shouldKick()) {
                allowed_kick_idx = testKicks(testing_copy);

                if (-1 != allowed_kick_idx) {
                        p_available_kicks = currentPart->getKicks();
                        p_kick_op = (*p_available_kicks)[allowed_kick_idx];

                        currentPart->rotate();
                        currentPart->move(p_kick_op[0]);
                        currentPart->moveVertical(p_kick_op[1]);
                } else {
                        // No allowed rotation possible
                }
        }
}

void Tetris::moveDown()
{
        last_time = 0;
}

void Tetris::drop()
{
        while (!isTouchingSide(TETRIS_SIDE_BOTTOM)) {
                step();
        }
}

int Tetris::getScore()
{
        return score;
}

int Tetris::getLines()
{
        return total_lines;
}

int Tetris::getLevel()
{
        return level + 1;
}

bool Tetris::isRunning(void)
{
        return isGameRunning;
}

bool Tetris::rowsToDelete(void) {
        for (int i = 0; ROWS > i; ++i) {
                if (should_delete_rows[i]) {
                        return true;
                }
        }
        return false;
}



void Tetris::process()
{
        if (isRunning()) {
                if (rowsToDelete()) {
                        animateDeleteRows(&Tetris::deleteFullRows);

                } else {
                        draw();

                        int64_t n = get_us();
                        double time_frame_s = (double) 1 / 60;
                        double seconds_cell = time_frame_s / g_per_cell[level] * 1000;

                        if (seconds_cell < (n - last_time)) {
                                last_time = n;
                                step();
                        }
                }


        }
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

void Tetris::deleteRowAndShiftDown(int line_to_delete)
{
        int i;
        for (i = line_to_delete; 0 < i; --i) {
                memcpy(grid[i], grid[i - 1], sizeof(grid[0]));
        }

        memset(grid[0], 0, sizeof(grid[0]));
}

int Tetris::getLinesToDelete(void)
{
        int i = 0;
        int j = 0;
        int deleted_rows = 0;

        for (i = ROWS;  0 <= i; --i) {

                bool should_delete_row = true;
                for (j = 0; COLUMNS > j; ++j) {
                        if (grid[i][j] != 1) {
                                should_delete_row = false;
                                break;
                        }
                }

                if (should_delete_row) {
                        deleted_rows++;
                        should_delete_rows[i] = should_delete_row;
                }
        }

        return deleted_rows;
}

void Tetris::addPartToGrid(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();

        int shapeHeight = shape.size();      // Number of rows in the shape
        int shapeWidth = shape[0].size();    // Number of columns in the shape

        if (!isOverlap(*currentPart)) {
                for (int i = 0; i < shapeHeight; ++i) {
                        for (int j = 0; j < shapeWidth; ++j) {
                                if (shape[i][j] == 1) {
                                        int gridRow = currentPart->getPositionY() + i;
                                        int gridCol = currentPart->getPositionX() + j;
                                        grid[gridRow][gridCol] = 1;  // Copy the value from the shape to the grid
                                }
                        }
                }
        }
}

bool Tetris::isCurrentPartPartiallyOutsideBounds(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();

        for (int i = 0; i < shapeHeight; ++i) {
                for (int j = 0; j < shapeWidth; ++j) {

                        if (shape[i][j] == 0) {
                                continue;
                        }

                        int gridRow = currentPart->getPositionY() + i;
                        int gridCol = currentPart->getPositionX() + j;

                        if (grid[gridRow][gridCol] == 1 || !(areCoordinatesInBounds(gridRow, gridCol))) {
                                return true;
                        }
                }
        }

        return false;
}

void Tetris::addScore(int deleted_lines)
{
        total_lines += deleted_lines;
        score += deleted_lines_multiplier[deleted_lines]*(level + 1);
        level = total_lines / 10;
}

void Tetris::step()
{
        int deleted_lines;
        if (isTouchingSide(TETRIS_SIDE_BOTTOM)) {
                if (isCurrentPartPartiallyOutsideBounds()) {
                        exit(0);
                }
                addPartToGrid();
                deleted_lines = getLinesToDelete();
                addScore(deleted_lines);
                addPart();
        } else {
                currentPart->moveVertical(-1);

                if (isTouchingGridContent()) {
                        exit(123);
                }
        }
}

void Tetris::draw(void)
{
        printCurrentPart();
        printNextPart();
        printGrid();
        printBufferGrid();

}

bool Tetris::isOverlap(Tetromino &tetromino)
{
        std::vector<std::vector<int>> const shape = tetromino.getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();

        for (int i = 0; i < shapeHeight; ++i) {
                for (int j = 0; j < shapeWidth; ++j) {

                        if (shape[i][j] == 0) {
                                continue;
                        }
                        int gridRow = tetromino.getPositionY() + i;
                        int gridCol = tetromino.getPositionX() + j;

                        if (grid[gridRow][gridCol] == 1 || !(areCoordinatesInBounds(gridRow, gridCol))) {
                                return true;
                        }
                }
        }

        return false;
}

int Tetris::testKicks(Tetromino &tetromino)
{

        auto const kicks = tetromino.getKicks();


        for (int test_idx = 0; 5 > test_idx; ++test_idx) {
                Tetromino testPart(tetromino);

                int moveHorizontal = (*kicks)[test_idx][0];
                int moveVertical = (*kicks)[test_idx][1];

                testPart.rotate();
                testPart.move(moveHorizontal);
                testPart.moveVertical(moveVertical);

                if (!isOverlap(testPart)) {
                        return test_idx;
                }
        }

        return -1;

}

void Tetris::clearBuffer(void)
{
        for (int i = 0; i < ROWS; ++i) {
                for (int j = 0; j < COLUMNS; ++j) {
                        buffer_grid[i][j] = 0;
                }
        }
}

bool Tetris::areCoordinatesInBounds(int x, int y)
{
        return (x >= 0 && x < ROWS && y >= 0 && y < COLUMNS);
}

void Tetris::printCurrentPart(void)
{

        std::vector<std::vector<int>> const shape = currentPart->getShape();

        int shapeHeight = shape.size();      // Number of rows in the shape
        int shapeWidth = shape[0].size();    // Number of columns in the shape
        clearBuffer();

        // Loop through each element in the shape matrix
        for (int i = 0; i < shapeHeight; ++i) {
                for (int j = 0; j < shapeWidth; ++j) {
                        // Calculate the corresponding position in the grid
                        int gridRow = currentPart->getPositionY() + i;
                        int gridCol = currentPart->getPositionX() + j;

                        // Ensure we're not going out of bounds in the grid
                        if (areCoordinatesInBounds(gridRow, gridCol)) {
                                buffer_grid[gridRow][gridCol] = grid[gridRow][gridCol];
                                if (shape[i][j] == 1) {
                                        buffer_grid[gridRow][gridCol] = 1;  // Copy the value from the shape to the grid
                                }
                        }
                }
        }
}

void Tetris::printBufferGrid(void)
{
        for (int i = 0; ROWS > i; ++i) {
                for (int j = 0; COLUMNS > j; ++j) {
                        if (0 != buffer_grid[i][j]) {
                                printer(i, j);
                        }
                }
        }
}

void Tetris::printNextPart(void)
{

        std::vector<std::vector<int>> const shape = nextPart->getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();

        for (int i = 0; shapeHeight > i; ++i) {
                for (int j = 0; shapeWidth > j; ++j) {
                        if (0 != shape[i][j]) {
                                next_printer(i, j);
                        }
                }
        }
}

void Tetris::printGrid(void)
{
        for (int i = 0; ROWS > i; ++i) {
                for (int j = 0; COLUMNS > j; ++j) {
                        if (0 != grid[i][j]) {
                                printer(i, j);
                        }
                }
        }
}


/*!
 * @brief Whether current part is touching with its specified side the window
 *        frame or any block at the grid
 *
 * @param partSide
 * @return
 */
bool Tetris:: isTouchingSide(Side_t const partSide)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();
        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();

        for (int i = 0; i < shapeHeight; ++i) {
                for (int j = 0; j < shapeWidth; ++j) {

                        if (shape[i][j] != 1) {
                                continue;
                        }
                        int gridCol = currentPart->getPositionX() + j;
                        int gridRow = currentPart->getPositionY() + i;

                        if ((TETRIS_SIDE_RIGHT == partSide) && (gridCol >= COLUMNS - 1)) {
                                return true;
                        } else if ((TETRIS_SIDE_LEFT == partSide) && (gridCol <= 0)) {
                                return true;
                        } else if ((TETRIS_SIDE_LEFT == partSide) && (grid[gridRow][gridCol - 1] == 1)) {
                                return true;
                        } else if ((TETRIS_SIDE_RIGHT == partSide) && (grid[gridRow][gridCol + 1] == 1)) {
                                return true;
                        } else if ((TETRIS_SIDE_BOTTOM == partSide) && (gridRow == ROWS - 1) && (1 == shape[i][j])) {
                                return true;
                        } else if ((TETRIS_SIDE_BOTTOM == partSide) && (1 == shape[i][j]) &&
                                   (grid[gridRow + 1][gridCol] == 1)) {
                                return true;
                        }
                }
        }

        return false;
}

bool Tetris::isTouchingGridContent(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();
        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();

        for (int i = 0; i < shapeHeight; ++i) {
                for (int j = 0; j < shapeWidth; ++j) {

                        if (shape[i][j] != 1) {
                                continue;
                        }
                        int gridCol = currentPart->getPositionX() + j;
                        int gridRow = currentPart->getPositionY() + i;

                        if (gridRow < 0) {
                                // Part is spawning, ignore it
                               // continue;
                        }
                        if (grid[gridRow][gridCol] == 1) {
                                return true;
                        }
                }
        }

        return false;
}


void Tetris::animateDeleteRows(void (Tetris::*onAnimationEnd)(void))
{
        int64_t n = get_us();
        bool state;
        if (50 < (n - last_time)) {
                last_time = n;
                iteration++;
        }

        switch (iteration) {
                case 0:
                        state = false;
                        break;
                case 1:
                        state = true;

                        break;
                case 2:
                        state = false;

                        break;
                case 3:
                        state = true;

                        break;
                case 4:
                        state = false;

                        break;
                case 5:
                        state = true;

                        break;

                case 6:
                        state = true;
                        break;


        }

        for (int i = 0; ROWS > i; ++i) {
                if(should_delete_rows[i]) {
                        setRow(state, i);
                }
        }

        if (iteration == 6) {
                iteration = 0;
                (this->*onAnimationEnd)();
        }

        printGrid();
}

void Tetris::setRow(bool b, int i)
{
        int value = b ? 1 : 0;
        for (int j = 0; COLUMNS > j; ++j) {
                grid[i][j] = value;
        }
}

void Tetris::deleteFullRows(void)
{
        int i = 0;
        int j = 0;
        bool deleted_a_row = true;

        while (deleted_a_row) {
                deleted_a_row = false;
                for (i = ROWS; (0 <= i) && (!deleted_a_row); --i) {
                        bool should_delete_row = true;
                        for (j = 0; COLUMNS > j; ++j) {
                                if (grid[i][j] != 1) {
                                        should_delete_row = false;
                                        break;
                                }
                        }

                        if (should_delete_row) {
                                deleted_a_row = true;
                                deleteRowAndShiftDown(i);
                        }
                }
        }

        memset(should_delete_rows, false, sizeof(should_delete_rows));

}
/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */



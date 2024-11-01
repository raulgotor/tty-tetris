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

Tetris::Tetris(std::function<void(int x, int y)> fprinter, std::function<int64_t(void)> _get_us)
{
        printer = fprinter;
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

void Tetris::addPart(void)
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
        std::unique_ptr<Shape> shape = shapeFactories[random_number]();

        Tetromino * part = new Tetromino(std::move(shape));
        this->currentPart = part;
}

void Tetris::startGame()
{
        score = 0;
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

int Tetris::getScore()
{
        return score;
}

bool Tetris::isRunning(void)
{
        return isGameRunning;
}

void Tetris::process()
{
        if (isRunning()) {
                int64_t n = get_us();
                if (300 < (n - last_time)) {
                        last_time = get_us();
                        step();
                }
                draw();
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

void Tetris::deleteFullLines(void)
{
        int i = 0;
        int j = 0;
        bool should_delete_rows[ROWS] = {false};
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
                                deleteRowAndShiftDown(i);
                                score += 10;
                                deleted_a_row = true;
                        }

                        should_delete_rows[i] = should_delete_row;
                }
        }

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

        return;
}

void Tetris::step()
{
        if (isTouchingSide(TETRIS_SIDE_BOTTOM)) {
                addPartToGrid();
                deleteFullLines();
                addPart();
        } else {
                currentPart->moveVertical(-1);
        }
}

void Tetris::draw(void)
{
        printCurrentPart();
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
bool Tetris::isTouchingSide(Side_t const partSide)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();
        int shapeHeight = shape.size();      // Number of rows in the shape
        int shapeWidth = shape[0].size();    // Number of columns in the shape

        // Loop through each element in the shape matrix
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

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */



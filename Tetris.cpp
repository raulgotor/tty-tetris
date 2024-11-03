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

static std::unique_ptr<Shape> getRandomShape(void);

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

Tetris::Tetris(std::function<void(int x, int y)> fprinter,
               std::function<void(int x, int y)> fnextPrinter,
               std::function<int64_t(void)> _getMs)
{
        printer = fprinter;
        next_printer = fnextPrinter;
        getMs = _getMs;
}

void Tetris::moveLeft(void)
{
        if (!isPartTouchingSide(TETRIS_SIDE_LEFT)) {
                currentPart->move(-1);
        }
}


void Tetris::moveRight(void)
{
        if (!isPartTouchingSide(TETRIS_SIDE_RIGHT)) {
                currentPart->move(1);
        }
}

void Tetris::startGame(void)
{
        score = 0;
        level = 0;
        totalLines = 0;
        clearBuffer();
        clearGrid();
        this->nextPart = new Tetromino(std::move(getRandomShape()));

        addPart();
        isGameRunning = true;
}


void Tetris::rotate(void)
{
        Tetromino testingCopy(*currentPart);
        int const (* pAvailableKicks)[5][2];
        int const (* pKickOp);
        int allowedKickIdx;

        if (currentPart->shouldKick()) {
                allowedKickIdx = testPartKicks(testingCopy);

                if (-1 != allowedKickIdx) {
                        pAvailableKicks = currentPart->getKicks();
                        pKickOp = (*pAvailableKicks)[allowedKickIdx];

                        currentPart->rotate();
                        currentPart->move(pKickOp[0]);
                        currentPart->moveVertical(pKickOp[1]);
                } else {
                        // No allowed rotation possible
                }
        }
}

void Tetris::moveDown(void)
{
        lastTime = 0;
}

void Tetris::drop(void)
{
        while (!isPartTouchingSide(TETRIS_SIDE_BOTTOM)) {
                step();
        }
}

int Tetris::getScore(void) const
{
        return score;
}

int Tetris::getLines(void) const
{
        return totalLines;
}

int Tetris::getLevel(void) const
{
        return level + 1;
}

bool Tetris::isRunning(void) const
{
        return isGameRunning;
}

void Tetris::process(void)
{
        int64_t n;
        double timeFrameS = (double) 1 / 60;
        double secondsCell;

        if (isRunning()) {
                if (isAnyLineToDelete()) {
                        animateLineDeletion(&Tetris::deleteCompletedLines);

                } else {
                        draw();

                        n = getMs();
                        secondsCell = timeFrameS / g_per_cell[level] * 1000;

                        if (secondsCell < (n - lastTime)) {
                                lastTime = n;
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
void f();

void Tetris::addPart(void)
{
        this->currentPart = nextPart;
        this->nextPart = new Tetromino(std::move(getRandomShape()));
}

void Tetris::addScore(int const numberOfLines)
{
        totalLines += numberOfLines;
        score += deletedLinesMultiplier[numberOfLines] * (level + 1);
        level = totalLines / 10;
}

void Tetris::step()
{
        if (isPartTouchingSide(TETRIS_SIDE_BOTTOM)) {
                if (isPartOOB()) {
                        exit(0);
                }
                addPartToGrid();
                addScore(getCompleteLines());
                addPart();
        } else {
                currentPart->moveVertical(-1);

                if (isPartTouchingGridContent()) {
                        exit(123);
                }
        }
}

void Tetris::addPartToBuffer(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int gridRow;
        int gridCol;
        int i;
        int j;

        clearBuffer();

        for (i = 0; shapeHeight > i; ++i) {
                for (j = 0; shapeWidth > j; ++j) {
                        gridRow = currentPart->getPositionY() + i;
                        gridCol = currentPart->getPositionX() + j;

                        if (isPointInBounds(gridRow, gridCol)) {
                                bufferGrid[gridRow][gridCol] = grid[gridRow][gridCol];
                                if (1 == shape[i][j]) {
                                        bufferGrid[gridRow][gridCol] = 1;
                                }
                        }
                }
        }
}

void Tetris::addPartToGrid(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int gridRow;
        int gridCol;
        int i;
        int j;

        if (!isOverlap(*currentPart)) {
                for (i = 0; shapeHeight > i; ++i) {
                        for (j = 0; shapeWidth > j; ++j) {
                                if (1 == shape[i][j]) {
                                        gridRow = currentPart->getPositionY() + i;
                                        gridCol = currentPart->getPositionX() + j;
                                        grid[gridRow][gridCol] = 1;
                                }
                        }
                }
        }
}

void Tetris::clearBuffer(void)
{
        int i;
        int j;

        for (i = 0; ROWS > i; ++i) {
                for (j = 0; COLUMNS > j; ++j) {
                        bufferGrid[i][j] = 0;
                }
        }
}

void Tetris::clearGrid(void)
{
        int i;
        int j;

        for (i = 0; ROWS > i; ++i) {
                for (j = 0; COLUMNS > j; ++j) {
                        grid[i][j] = 0;
                }
        }
}

void Tetris::draw(void)
{
        addPartToBuffer();
        drawNextPart();
        drawGrid();
        drawBuffer();

}

void Tetris::drawBuffer(void)
{
        int i;
        int j;

        for (i = 0; ROWS > i; ++i) {
                for (j = 0; COLUMNS > j; ++j) {
                        if (0 != bufferGrid[i][j]) {
                                printer(i, j);
                        }
                }
        }
}

void Tetris::drawGrid(void)
{
        int i;
        int j;

        for (i = 0; ROWS > i; ++i) {
                for (j = 0; COLUMNS > j; ++j) {
                        if (0 != grid[i][j]) {
                                printer(i, j);
                        }
                }
        }
}

void Tetris::drawNextPart(void)
{
        std::vector<std::vector<int>> const shape = nextPart->getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int i;
        int j;

        for (i = 0; shapeHeight > i; ++i) {
                for (j = 0; shapeWidth > j; ++j) {
                        if (0 != shape[i][j]) {
                                next_printer(i, j);
                        }
                }
        }
}

// TODO: do something not as horrible as this here
void Tetris::animateLineDeletion(void (Tetris::*onAnimationEnd)(void))
{
        int64_t n = getMs();
        bool state;
        int i;

        if (50 < (n - lastTime)) {
                lastTime = n;
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

        for (i = 0; ROWS > i; ++i) {
                if(shouldDeleteRows[i]) {
                        setLineAtGrid(state, i);
                }
        }

        if (iteration == 6) {
                iteration = 0;
                (this->*onAnimationEnd)();
        }

        drawGrid();
}

void Tetris::deleteCompletedLines(void)
{
        int i = 0;
        int j = 0;
        bool deletedARow = true;
        bool shouldDeleteRow;

        while (deletedARow) {
                deletedARow = false;
                for (i = ROWS; (0 <= i) && (!deletedARow); --i) {
                        shouldDeleteRow = true;

                        for (j = 0; COLUMNS > j; ++j) {
                                if (grid[i][j] != 1) {
                                        shouldDeleteRow = false;
                                        break;
                                }
                        }

                        if (shouldDeleteRow) {
                                deletedARow = true;
                                deleteLineAndShiftDown(i);
                        }
                }
        }

        memset(shouldDeleteRows, false, sizeof(shouldDeleteRows));
}

void Tetris::deleteLineAndShiftDown(int const lineToDelete)
{
        int i;

        for (i = lineToDelete; 0 < i; --i) {
                memcpy(grid[i], grid[i - 1], sizeof(grid[0]));
        }

        memset(grid[0], 0, sizeof(grid[0]));
}

int Tetris::getCompleteLines(void)
{
        int i = 0;
        int j = 0;
        int deletedRows = 0;
        bool shouldDeleteRow;

        for (i = ROWS;  0 <= i; --i) {
                shouldDeleteRow = true;

                for (j = 0; COLUMNS > j; ++j) {
                        if (grid[i][j] != 1) {
                                shouldDeleteRow = false;
                                break;
                        }
                }

                if (shouldDeleteRow) {
                        deletedRows++;
                        shouldDeleteRows[i] = shouldDeleteRow;
                }
        }

        return deletedRows;
}

bool Tetris::isAnyLineToDelete(void) {
        for (int i = 0; ROWS > i; ++i) {
                if (shouldDeleteRows[i]) {
                        return true;
                }
        }
        return false;
}

void Tetris::setLineAtGrid(bool const b, int const i)
{
        int j;
        int value = b ? 1 : 0;
        for (j = 0; COLUMNS > j; ++j) {
                grid[i][j] = value;
        }
}


bool Tetris::isOverlap(Tetromino &tetromino)
{
        std::vector<std::vector<int>> const shape = tetromino.getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int gridRow;
        int gridCol;
        int i;
        int j;

        for (i = 0; shapeHeight > i; ++i) {
                for (j = 0; shapeWidth > j; ++j) {

                        if (0 == shape[i][j]) {
                                continue;
                        }

                        gridRow = tetromino.getPositionY() + i;
                        gridCol = tetromino.getPositionX() + j;

                        if ((1 == grid[gridRow][gridCol]) || !(isPointInBounds(gridRow, gridCol))) {
                                return true;
                        }
                }
        }

        return false;
}

bool Tetris::isPartOOB(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();

        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int gridRow;
        int gridCol;
        int i;
        int j;

        for (i = 0; shapeHeight > i; ++i) {
                for (j = 0; shapeWidth > j; ++j) {

                        if (shape[i][j] == 0) {
                                continue;
                        }

                        gridRow = currentPart->getPositionY() + i;
                        gridCol = currentPart->getPositionX() + j;

                        if ((1 == grid[gridRow][gridCol]) || !(isPointInBounds(gridRow, gridCol))) {
                                return true;
                        }
                }
        }

        return false;
}

bool Tetris::isPartTouchingGridContent(void)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();
        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int gridRow;
        int gridCol;
        int i;
        int j;

        for (i = 0; i < shapeHeight; ++i) {
                for (j = 0; j < shapeWidth; ++j) {

                        if (shape[i][j] != 1) {
                                continue;
                        }

                        gridCol = currentPart->getPositionX() + j;
                        gridRow = currentPart->getPositionY() + i;

                        if (0 > gridRow) {
                                // Part is spawning, ignore it
                                // continue;
                        }

                        if (1 == grid[gridRow][gridCol]) {
                                return true;
                        }
                }
        }

        return false;
}

/*!
 * @brief Whether current part is touching with its specified side the window
 *        frame or any block at the grid
 *
 * @param partSide
 * @return
 */
bool Tetris:: isPartTouchingSide(Side_t const partSide)
{
        std::vector<std::vector<int>> const shape = currentPart->getShape();
        int shapeHeight = shape.size();
        int shapeWidth = shape[0].size();
        int gridRow;
        int gridCol;
        int i;
        int j;

        for (i = 0; shapeHeight > i; ++i) {
                for (j = 0; shapeWidth > j; ++j) {
                        if (1 != shape[i][j]) {
                                continue;
                        }

                        gridCol = currentPart->getPositionX() + j;
                        gridRow = currentPart->getPositionY() + i;

                        if ((TETRIS_SIDE_RIGHT == partSide) && (gridCol >= COLUMNS - 1)) {
                                return true;
                        } else if ((TETRIS_SIDE_LEFT == partSide) && (0 >= gridCol)) {
                                return true;
                        } else if ((TETRIS_SIDE_LEFT == partSide) && (1 == grid[gridRow][gridCol - 1])) {
                                return true;
                        } else if ((TETRIS_SIDE_RIGHT == partSide) && (1 == grid[gridRow][gridCol + 1])) {
                                return true;
                        } else if ((TETRIS_SIDE_BOTTOM == partSide) && ((ROWS - 1) == gridRow) && (1 == shape[i][j])) {
                                return true;
                        } else if ((TETRIS_SIDE_BOTTOM == partSide) && (1 == shape[i][j]) &&
                                   (grid[gridRow + 1][gridCol] == 1)) {
                                return true;
                        }
                }
        }

        return false;
}

bool Tetris::isPointInBounds(int const x, int const y)
{
        return ((0 <= x) && (ROWS > x) && (0 <= y) && (COLUMNS > y));
}

int Tetris::testPartKicks(Tetromino &tetromino)
{
        auto const kicks = tetromino.getKicks();
        int testIdx;
        int moveHorizontal;
        int moveVertical;

        for (testIdx = 0; 5 > testIdx; ++testIdx) {
                Tetromino testPart(tetromino);

                moveHorizontal = (*kicks)[testIdx][0];
                moveVertical = (*kicks)[testIdx][1];

                testPart.rotate();
                testPart.move(moveHorizontal);
                testPart.moveVertical(moveVertical);

                if (!isOverlap(testPart)) {
                        return testIdx;
                }
        }

        return -1;
}

static std::unique_ptr<Shape> getRandomShape(void)
{
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

        int randomNumber = (std::rand() % 6);

        return shapeFactories[randomNumber]();
}
/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */

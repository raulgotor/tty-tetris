/*!
 *******************************************************************************
 * @file Tetromino.cpp
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

#include "Tetromino.h"

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

/*
 *******************************************************************************
 * Public Function Bodies                                                      *
 *******************************************************************************
 */

std::vector<std::vector<int>> Tetromino::getShape()
{
        if (nullptr != shape) {
                return performNRotations(rotation);;
        } else {
                return std::vector<std::vector<int>>();
        }
}

void Tetromino::rotate(void) {
        ++rotation;

        if (4 <= rotation) {
                rotation = 0;
        }
}

void Tetromino::moveVertical(int direction)
{
        positionY -= direction;
}

void Tetromino::move(int direction) {
        positionX += direction;
}

int Tetromino::getPositionX()
{
        return positionX;
}

int Tetromino::getPositionY()
{
        return positionY;
}

bool Tetromino::shouldKick(void)
{
        return shape->shouldKick();
}

const int (*Tetromino::getKicks() const)[5][2] {
        const int (*p)[4][5][2]  = shape->getKicks();

        return &((*p)[rotation]);
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

std::vector<std::vector<int>> Tetromino::performNRotations(int n)
{
        std::vector<std::vector<int>> s = shape->getShape();

        for (int i = 0; i < n; ++i) {
                transpose(s);
        }

        return s;
}

void Tetromino::transpose(std::vector<std::vector<int>>& matrix) {

        unsigned long width = matrix.size();

        for (int i = 0; width > i; ++i) {
                for (int j = i + 1; j < width; ++j) {
                        std::swap(matrix[i][j], matrix[j][i]);
                }
        }

        for (int i = 0; width > i; ++i) {
                std::reverse(matrix[i].begin(), matrix[i].end());
        }
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */


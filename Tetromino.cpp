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

void Tetromino::moveVertical(int const direction)
{
        positionY -= direction;
}

void Tetromino::move(int const direction) {
        positionX += direction;
}

int Tetromino::getPositionX(void) const
{
        return positionX;
}

int Tetromino::getPositionY(void) const
{
        return positionY;
}

bool Tetromino::shouldKick(void)
{
        return shape->shouldKick();
}

int const (*Tetromino::getKicks() const)[5][2] {
        int const (*p)[4][5][2]  = shape->getKicks();

        return &((*p)[rotation]);
}

/*
 *******************************************************************************
 * Private Function Bodies                                                     *
 *******************************************************************************
 */

std::vector<std::vector<int>> Tetromino::performNRotations(int const n)
{
        std::vector<std::vector<int>> s = shape->getShape();
        int i;

        for (i = 0; n > i; ++i) {
                transpose(s);
        }

        return s;
}

void Tetromino::transpose(std::vector<std::vector<int>>& matrix) {

        unsigned long width = matrix.size();
        int i;
        int j;

        for (i = 0; width > i; ++i) {
                for (j = i + 1; j < width; ++j) {
                        std::swap(matrix[i][j], matrix[j][i]);
                }
        }

        for (i = 0; width > i; ++i) {
                std::reverse(matrix[i].begin(), matrix[i].end());
        }
}

/*
 *******************************************************************************
 * Interrupt Service Routines / Tasks / Thread Main Functions                  *
 *******************************************************************************
 */


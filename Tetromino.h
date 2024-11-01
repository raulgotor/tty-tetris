/*!
 *******************************************************************************
 * @file Tetromino.h
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

#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>
#include "Shape.h"

class Tetromino {

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
        std::unique_ptr<Shape> shape;
        int positionY = 0;
        int positionX = 0;
        int rotation = 0;

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

/*
 *******************************************************************************
 * Public Function Prototypes                                                  *
 *******************************************************************************
 */

public:
        Tetromino(std::unique_ptr<Shape> s) : shape(std::move(s))
        {}

        Tetromino(const Tetromino &rhs)
        {
                this->shape = rhs.shape->clone();
                this->positionY = rhs.positionY;
                this->positionX = rhs.positionX;
                this->rotation = rhs.rotation;
        }

        void rotate(void);

        void moveVertical(int direction);

        void move(int direction);

        bool shouldKick(void);

        const int (* getKicks() const )[5][2];

        std::vector<std::vector<int>> getShape();

        int getPositionX();

        int getPositionY();

/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

private:

        std::vector<std::vector<int>> performNRotations(int n);

        static void transpose(std::vector<std::vector<int>> &matrix);
};


#endif //TETROMINO_H

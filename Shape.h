/*!
 *******************************************************************************
 * @file Shape.h
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

#ifndef SHAPE_H
#define SHAPE_H

#include <vector>

class Shape {

/*
 *******************************************************************************
 * Public Constants                                                            *
 *******************************************************************************
 */

public:

        int kicks_data[4][5][2] = {
                {{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}},
                {{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}},
                {{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}},
                {{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}},
        };

/*
 *******************************************************************************
 * Private Constants                                                           *
 *******************************************************************************
 */

private:

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

protected:
        int width;
        int height;
        bool _shouldKick;

        std::vector<std::vector<int>> shape;
        const int (* kicks)[4][5][2] = nullptr;

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
        Shape(int w, int h) : width(w), height(h), shape(w, std::vector<int>(h))
        {}

        virtual ~Shape()
        {}

        Shape(const Shape &other) : width(other.width), height(other.height), shape(other.shape), kicks(other.kicks),
                                    _shouldKick(other._shouldKick)
        {}

        Shape &operator=(const Shape &rhs)
        {
                if (this != &rhs) {
                        this->width = rhs.width;
                        this->_shouldKick = rhs._shouldKick;
                        this->height = rhs.height;
                        this->shape = rhs.shape;
                        this->kicks = rhs.kicks;
                }
                return *this;
        }

        virtual const int (* getKicks() const )[4][5][2]
        {
                return &kicks_data;
        }

        virtual std::unique_ptr<Shape> clone() const = 0;

        virtual void initializeShape() = 0;

        bool shouldKick(void)
        {
                return _shouldKick;
        }

        std::vector<std::vector<int>> getShape() const
        {
                return shape;
        }

/*
 *******************************************************************************
 * Private Function Prototypes                                                 *
 *******************************************************************************
 */

private:

};


class TShape : public Shape {
public:
        TShape() : Shape(3, 3)
        {
                initializeShape();
                _shouldKick = true;
        }

        void initializeShape() override
        {
                shape = {
                        {0, 1, 0},
                        {1, 1, 1},
                        {0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<TShape>(*this);
        }
};

class SShape : public Shape {
public:
        SShape() : Shape(3, 3)
        {
                initializeShape();
                _shouldKick = true;
        }

        void initializeShape() override
        {
                shape = {
                        {0, 1, 1},
                        {1, 1, 0},
                        {0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<SShape>(*this);
        }
};


class ZShape : public Shape {
public:
        ZShape() : Shape(3, 3)
        {
                initializeShape();
                _shouldKick = true;
        }

        void initializeShape() override
        {
                shape = {
                        {1, 1, 0},
                        {0, 1, 1},
                        {0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<ZShape>(*this);
        }
};

class IShape : public Shape {
public:
        int I_kicks_data[4][5][2] = {
                {{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}},
                {{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}},
                {{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}},
                {{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}},
        };

        IShape() : Shape(4, 4)
        {
                initializeShape();
                _shouldKick = true;
        }

        const int (* getKicks() const )[4][5][2] override {
                return &I_kicks_data;
        }

        void initializeShape() override
        {
                shape = {
                        {0, 0, 0, 0},
                        {1, 1, 1, 1},
                        {0, 0, 0, 0},
                        {0, 0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<IShape>(*this);
        }
};

class JShape : public Shape {
public:
        JShape() : Shape(4, 4)
        {
                initializeShape();
                _shouldKick = true;
                kicks = &kicks_data;
        }

        void initializeShape() override
        {
                shape = {
                        {1, 0, 0},
                        {1, 1, 1},
                        {0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<JShape>(*this);
        }
};

class LShape : public Shape {
public:
        LShape() : Shape(4, 4)
        {
                initializeShape();
                _shouldKick = true;
                kicks = &kicks_data;
        }

        void initializeShape() override
        {
                shape = {
                        {0, 0, 1},
                        {1, 1, 1},
                        {0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<LShape>(*this);
        }
};


class OShape : public Shape {
public:
        OShape() : Shape(4, 3)
        {
                initializeShape();
                _shouldKick = false;
        }

        void initializeShape() override
        {
                shape = {
                        {0, 1, 1, 0},
                        {0, 1, 1, 0},
                        {0, 0, 0, 0},
                };
        }

        std::unique_ptr<Shape> clone() const override
        {
                return std::make_unique<OShape>(*this);
        }
};

#endif //SHAPE_H

#ifndef CELL_CONTENT_HPP
#define CELL_CONTENT_HPP

#include <random>

#include "common.hpp"
#include "point.hpp"
#include "shape.hpp"

// Classes from grid.hpp
class Grid;
class Cell;

/**
 * CellContent, base class of everything that can go on a Cell
 *
 * @param grid grid the cell (on wich the content is put) is part of
 * @param cell cell on which the content is put
 * @param drawable the shape of the content
 * @param movable should the content be moved
 * @param matchable can the content be matched with others
 * @param clearable should it be clearable
 */
class CellContent : public DrawableContainer
{
    protected:
        Grid &grid;
        Cell *containerCell;

        // Properties
        bool movable;
        bool matchable;
        bool clearable;

        // Animations states
        bool moveFinished = false;
        bool clearFinished = false;
    public:
        CellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable, bool movable, bool matchable, bool clearable = true);

        CellContent(const CellContent& c) = delete;
        CellContent operator=(const CellContent& c) = delete;

        bool isMovable() const { return movable; }
        bool isMatchable() const { return matchable; }
        bool isClearable() const { return clearable; }

        virtual void clear();

        // State of animation
        bool isClearing();
        bool isMoving();

        void moveTo(const Point &cell);
        void animationFinished(AnimationT a) override;

        // Used in combinations
        virtual bool operator==(CellContent &other) const = 0;
};

// A CellContent object that is matchable should be
// an instance of a derived class from MatchableCellContent.
/* class MatchableCellContent : public CellContent */
/* { */
/*     public: */
/*         MatchableCellContent(bool movable) : CellContent(movable, true) { } */
/*         virtual bool hasMatch(const T &other) const = 0; */
/* }; */

/**
 * A standard candy
 *
 * It has no powers, can fall, be matched, etc.
 * It is represented by a square, for now.
 *
 * If no color is provided, a random one is chosen.
 *
 * @param grid grid that contains it
 * @param cell cell that contains it
 * @param center its position on the window
 * @param side size of its side
 */
class StandardCandy : public CellContent
{
    public:
        // This is different than FL colors
        // because only these are permitted.
        enum class Color {
            Blue,
            Green,
            Cyan,
            Magenta,
            Red,
            Yellow
        };
        static constexpr Fl_Color flRelative[6] = {
            FL_BLUE,
            FL_GREEN,
            FL_DARK_CYAN,
            FL_MAGENTA,
            FL_RED,
            FL_YELLOW
        };
    protected:
        Color color;  // identifier of a candy

        StandardCandy(Grid &grid, Cell *cell, Color color, std::shared_ptr<AnimatableShape> shape);
    public:
        StandardCandy(Grid &grid, Cell *cell, Point center, int side);
        StandardCandy(Grid &grid, Cell *cell, Point center, int side, Color color);

        /* virtual bool hasMatch(const StandardCandy &other) const override; TODO implement subclasses */

        void draw() override;

        virtual bool operator==(CellContent &other) const override;  // TODO replace with hasmatchwith

        // Getters
        Color getColor() const { return color; }
};

class StripedCandy : public StandardCandy
{
    protected:
        Axis axis;  // wheter horizontal or vertical
    public:
        /* StripedCandy(Grid &grid, Cell *cell, Point center, int side); */
        /* StripedCandy(Grid &grid, Cell *cell, Point center, int side, Direction direction); */
        StripedCandy(Grid &grid, Cell *cell, Point center, int side, Color color, Axis axis);

        void clear() override;
};

#endif

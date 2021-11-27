#ifndef CELL_CONTENT_HPP
#define CELL_CONTENT_HPP

#include <random>

#include "common.hpp"
#include "point.hpp"
#include "shape.hpp"

// Classes from grid.hpp
class Grid;
class Cell;

enum class ContentT {
    StandardCandy,
    StripedCandy,
    WrappedCandy,
    WrappedCandySecPhase,
    ColourfulBomb,
    Wall
};

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
        Grid &grid;           // reference because always the same
        Cell *containerCell;  // may change, when cellContent is moved (if movable)
    public:
        CellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable);

        CellContent(const CellContent& c) = delete;
        CellContent operator=(const CellContent& c) = delete;

        /* bool isClearable() const { return std::dynamic_pointer_cast<ClearableCellContent>(this)} */
        /* bool isMovable() const { return movable; } */
        /* bool isMatchable() const { return matchable; } */
};

class Wall : public CellContent
{
    public:
        Wall(Grid &grid, Cell *cell, const Point &center, int side);
};

// A CellContent object that is matchable should be
// an instance of a derived class from MatchableCellContent.

class ClearableCellContent : public virtual CellContent
{
    protected:
        bool clearableByOther;

        // Animations states
        bool clearFinished = false;
        bool m_isClearing = false;
    public:
        ClearableCellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable, bool clearableByOther);

        void draw() override;

        virtual void clear();
        virtual void clearWithoutAnimation();

        // State of animation
        bool isClearing() { return m_isClearing; }

        void animationFinished(AnimationT a) override;
};

class MovableCellContent : public virtual CellContent
{
    protected:
        // Animations states
        bool moveFinished = false;
        bool m_isMoving = false;
    public:
        MovableCellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable);

        void draw() override;

        virtual void moveTo(const Point &point);
        virtual void moveToWithoutAnimation(const Point &point);

        // State of animation
        bool isMoving() { return m_isMoving; }

        void animationFinished(AnimationT a) override;
};

/**
 * A CellContent object that is matchable should be
 * an instance of a derived class from MatchableCellContent.
 */
class MatchableCellContent : public virtual CellContent
{
    public:
        MatchableCellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable);

        virtual bool hasMatchWith(const Point &point) const = 0;
};

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
class StandardCandy : public ClearableCellContent, public MovableCellContent, public MatchableCellContent
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
            0x22a0fd00,  // Blue
            0x4ad81200,  // Green
            0xfe810200,  // Orange
            0xd31ded00,  // Purple
            0xe3010200,  // Red
            /* 0xfad40000   // Yellow */
            0xFFFF8A00   // Yellow
        };
    protected:
        Color color;  // identifier of a candy

        // This constructor is for derived classes to change appearance of candy
        StandardCandy(Grid &grid, Cell *cell, Color color, std::shared_ptr<AnimatableShape> shape);
    public:
        StandardCandy(Grid &grid, Cell *cell, Point center, int side);
        StandardCandy(Grid &grid, Cell *cell, Point center, int side, Color color);

        bool hasMatchWith(const Point &point) const override;

        void draw() override;

        void animationFinished(AnimationT a) override;
        /* virtual bool operator==(CellContent &other) const override;  // TODO replace with hasmatchwith */

        // Getters
        Color getColor() const { return color; }
};

class StripedCandy : public StandardCandy
{
    protected:
        Axis axis;  // wether horizontal or vertical
    public:
        StripedCandy(Grid &grid, Cell *cell, Point center, int side, Color color, Axis axis);

        void clearWithoutAnimation() override;
};

class WrappedCandy : public StandardCandy
{
    public:
        WrappedCandy(Grid &grid, Cell *cell, Point center, int side, Color color);

        void clearWithoutAnimation() override;
};

class WrappedCandySecPhase : public StandardCandy
{
public:
    WrappedCandySecPhase(Grid &grid, Cell *cell, Point center, int side, Color color);

    void clearWithoutAnimation() override;
};

#endif

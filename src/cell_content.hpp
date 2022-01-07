#ifndef CELL_CONTENT_HPP
#define CELL_CONTENT_HPP

#include <random>

#include "common.hpp"
#include "point.hpp"
#include "shape.hpp"
#include "event.hpp"

// Classes from grid.hpp
class Grid;
class Cell;

/// enumerates all kinds of elements that a cell could contain
enum class ContentT {
    StandardCandy,
    StripedCandy,
    WrappedCandy,
    ColourBomb,
    Wall,
    TextContent,
    Icing,
    Fruit
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

        bool m_isPulsing {false};
        bool m_pulseFinished {false};
    public:
        CellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable);

        CellContent(const CellContent& c) = delete;
        CellContent operator=(const CellContent& c) = delete;

        void animationFinished(AnimationT) override;

        virtual void update(Event) { }

        virtual ContentT getType() = 0;

        void draw() override;

        /* bool isClearable() const { return std::dynamic_pointer_cast<ClearableCellContent>(this)} */
        /* bool isMovable() const { return movable; } */
        /* bool isMatchable() const { return matchable; } */
};

class TextContent : public CellContent
{
    public:
        TextContent(Grid &grid, Cell *cell, const Point &center, int side, std::string text, int fontSize = 20, Fl_Color textColor = FL_WHITE);

        ContentT getType() override { return ContentT::TextContent; }
        std::string getString() const { return dynamic_pointer_cast<Text>(drawable)->getString(); }
};

/**
 * Wall that can be contained in a cell.
 *
 * Is neither movable nor clearable.
 */
class Wall : public CellContent
{
    public:
        Wall(Grid &grid, Cell *cell, const Point &center, int side);

        ContentT getType() override { return ContentT::Wall; }
};

/**
 * Class that implements the clearable
 * part of a game element.
 *
 * All game elements that are clearable
 * should inherit from this class
 */
class ClearableCellContent : public virtual CellContent
{
    protected:
        bool clearableByOther;

        // Animations states
        bool clearFinished = false;
        bool m_isClearing = false;

        bool clearAtFallEnd{false};
    public:
        ClearableCellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable, bool clearableByOther);

        void draw() override;

        virtual void clearWithoutEffect();
        virtual void clear();
        virtual void clearWithoutAnimation();

        // State of animation
        bool isClearing() { return m_isClearing; }

        void animationFinished(AnimationT a) override;
};

/**
 * Icing that can be contained in a cell.
 *
 * Is neither movable nor matchable, but is well clearable.
 */
class Icing : public ClearableCellContent
{
    private:
        int layers;
        Text num;
    public:
        Icing(Grid &grid, Cell *cell, const Point &center, int side, int layers = 2);

        int getLayers() const { return layers; }
        void removeLayer();

        void clear() override;
        void update(Event e) override;

        void draw() override;

        ContentT getType() override { return ContentT::Icing; }
};

/**
 * Class that implements the movable
 * part of a game element.
 *
 * All game elements that are movable
 * should inherit from this class
 */
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

        virtual void wasSwappedWith(const Point &p);

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

class Fruit : public ClearableCellContent, public MovableCellContent
{
public:
    enum class Type {
        Cherry = 4,
        Hazelnut = 6
    };

protected:
    Type type;
    Fruit(Grid &grid, Cell *cell, const Point &center, std::shared_ptr<AnimatableShape> shape);
    bool isAtBottom();

public:
    Fruit(Grid &grid, Cell *cell, const Point &center, int side, Type type);

    void clear() override;
    void update(Event e) override;

    void draw() override;

    void animationFinished(AnimationT a) override;

    ContentT getType() override { return ContentT::Fruit; }
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

        // Getters
        Color getColor() const { return color; }

        void clearWithoutAnimation() override;

        ContentT getType() override { return ContentT::StandardCandy; }
};

/**
 * Stripped candy
 *
 * Uses the StrippedRectangle shape.
 *
 * Special type of candy,
 * uses the basic methods of StandardCandy,
 * but some others are overridden to make
 * the candy act like he should be.
 */
class StripedCandy : public StandardCandy
{
    protected:
        Axis axis;  // wether horizontal or vertical

        // clearWithoutAnimation uses this to know the way he must explode
        bool doubleStriped {false};
        bool wrappedWithStriped {false};

        void doubleStripedClear();
        void wrappedWithStripedClear();
        void regularClear();

    public:
        StripedCandy(Grid &grid, Cell *cell, Point center, int side, Color color, Axis axis);
        StripedCandy(Grid &grid, Cell *cell, Point center, int side, Color color);

        void clearWithoutAnimation() override;

        void wasSwappedWith(const Point &p) override;

        ContentT getType() override { return ContentT::StripedCandy; }
};

/**
 * Wrapped candy
 *
 * Uses the Star shape.
 * Has two phases, explodes twice.
 *
 * Special type of candy,
 * uses the basic methods of StandardCandy,
 * but some others are overridden to make
 * the candy act like he should be.
 */
class WrappedCandy : public StandardCandy
{
    private:
        bool secondPhase {false};

        // clearWithoutAnimation uses this to know the way he must explode
        bool doubleWrapped {false};
        bool wrappedWithStriped {false};

        void doubleWrappedClear();
        void wrappedWithStripedClear();
        void regularClear();

    public:
        WrappedCandy(Grid &grid, Cell *cell, Point center, int side, Color color);

        void clear() override;
        void clearWithoutAnimation() override;
        void wasSwappedWith(const Point &p) override;

        void update(Event e) override;
        ContentT getType() override { return  ContentT::WrappedCandy; }
};

/**
 * Colour Bomb
 *
 * Uses the MulticolourCircle shape.
 *
 * Special type of candy,
 * will act differently depending on
 * what candy he is switched with.
 */
class ColourBomb : public ClearableCellContent, public MovableCellContent
{
    private:
        StandardCandy::Color colorToReplace{};
        ContentT typeToReplaceWith {ContentT::StandardCandy};
        bool wasSwapped {false};

        void draw() override;
        void animationFinished(AnimationT a) override;

        StandardCandy::Color getColorToClear() { return static_cast<StandardCandy::Color>(std::rand()%6); }

        void replaceAndExplode();
    public:
        ColourBomb(Grid &grid, Cell *cell, Point center, int side);

        void clearWithoutAnimation() override;

        void wasSwappedWith(const Point &p) override;

        ContentT getType() override { return ContentT::ColourBomb; }
};

#endif

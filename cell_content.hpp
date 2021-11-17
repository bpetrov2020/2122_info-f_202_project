#ifndef CELL_CONTENT_HPP
#define CELL_CONTENT_HPP

#include <random>
#include "point.hpp"
#include "shape.hpp"


class CellContent
{
        /* bool isClearable; */
    protected:
        bool movable;
        bool matchable;
    public:
        CellContent(bool movable, bool matchable)
            : movable{movable}, matchable{matchable} { }
        virtual ~CellContent() noexcept = default;

        virtual void draw() = 0;
        virtual void setCenter(const Point) = 0;

        bool isMovable() const { return movable; }
        bool isMatchable() const { return matchable; }

        // To implement if matchable, else delete it
        /* virtual bool hasMatch(const T &other) const = 0; */

        // Used in combinations
        virtual bool operator==(CellContent &other) const = 0;
};

// A CellContent object that is matchable should be
// an instance of a derived class from MatchableCellContent.
/* template <typename T> */
/* class MatchableCellContent : public CellContent */
/* { */
/*     public: */
/*         MatchableCellContent(bool movable) : CellContent(movable, true) { } */
/*         virtual bool hasMatch(const T &other) const = 0; */
/* }; */

/* class StandardCandy : public MatchableCellContent<StandardCandy> */
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
    private:
        Rectangle drawableContent;
        Color color;  // identifier of a candy
        /* Grid grid; */
    public:
        StandardCandy(Point center, int side);
        StandardCandy(Point center, int side, Color c);

        virtual void draw() override;
        /* virtual bool hasMatch(const StandardCandy &other) const override; */

        virtual bool operator==(CellContent &other) const override;

        // Getters
        Color getColor() const { return color; }

        // Setters
        virtual void setCenter(const Point p) override { drawableContent.setCenter(p); }

        /* void clear(); */
};

#endif

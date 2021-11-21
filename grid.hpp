#ifndef GRID_HPP
#define GRID_HPP

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

#include "shape.hpp"
#include "colors.hpp"
#include "point.hpp"
#include "cell_content.hpp"
#include "common.hpp"

class Grid;

/**
 * A Cell, part of a grid
 *
 * A Cell and its content should
 * have the same center, the only exception
 * is in the case of animations.
 */
class Cell : public DrawableContainer, public Interactive
{
    protected:
        Grid &grid;
        Point index;  // position in the grid
        std::shared_ptr<CellContent> content;
        bool selected = false;
    public:
        Cell(Point center, int width, int height, Point index, Grid &grid);

        // Two functions are needed to have the background
        // and foreground drawn separately
        void draw() override;
        void drawContent();

        // Functions acting on the content of the cell
        void clear();
        void clearWithoutAnimation();
        bool isEmpty() const;
        bool isContentMovable() const;
        bool isClearing() { return content->isClearing(); }

        void moveContentTo(Cell &other);
        void swapContentWith(Cell &other);

        auto &getContent() { return content; }
        void setContent(std::shared_ptr<CellContent> c) { content = std::move(c); }

        // Functions acting on the relations between the cell and its grid
        bool isSelected() const { return selected; }

        const Point getIndex() const { return index; }

        bool toggleSelected();

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        bool hasContentAnimation() { return content ? content->hasAnimation() : false; }

        bool operator==(const Cell &other) { return index == other.index; }
};

/* class Grid : public DrawableContainer, public Interactive */
/* { */
/*     public: */
/*         enum class Direction { */
/*             South, */
/*             North, */
/*             West, */
/*             East, */
/*             SouthWest, */
/*             SouthEast, */
/*             NorthWest, */
/*             NorthEast */
/*         }; */
/*         constexpr static std::array<Point, 8> directionModifier { */
/*             Point{ 0, -1},  // South */
/*                 Point{ 0,  1},  // North */
/*                 Point{-1,  0},  // West */
/*                 Point{ 1,  0},  // East */
/*                 Point{-1, -1},  // SouthWest */
/*                 Point{ 1, -1},  // SouthEast */
/*                 Point{-1,  1},  // NorthWest */
/*                 Point{ 1,  1}   // NorthEast */
/*         }; */
/*     protected: */
/*         std::vector< std::vector<Cell> > matrix; */
/*         int selectedCount = 0; */
/*         int cellContentSide = 10; */
/*         bool isExclusive = false; */
/*     public: */
/*         Grid(Point center, int width, int height, int rows, int columns); */
/*         Grid(Point center, int width, int height, int side); */

/*         class Iterator { */
/*             private: */
/*                 Grid* g; */
/*                 Point coord; */
/*             public: */
/*                 Iterator(Grid* g, Point coord) : g{g}, coord{coord} { } */

/*                 Iterator &operator++() { */
/*                     int xs = static_cast<int>(g->matrix[0].size()); */
/*                     ++coord.x; */
/*                     coord.y += coord.x/xs; */
/*                     coord.x %= xs; */
/*                     return *this; */
/*                 } */
/*                 Cell &operator*() { return g->at(coord); } */
/*                 bool operator!=(const Iterator &other) { return coord != other.coord; } */
/*         }; */

/*         Iterator begin() { return Iterator{this, {0, 0}}; } */
/*         Iterator end() { return Iterator{this, {0, static_cast<int>(matrix.size())}}; } */

/*         // From interactive */
/*         void mouseMove(Point mouseLoc) override { for (auto &c: *this) c.mouseMove(mouseLoc); } */
/*         void mouseClick(Point mouseLoc) override { for (auto &c: *this) c.mouseClick(mouseLoc); } */
/*         void mouseDrag(Point mouseLoc) override { for (auto &c: *this) c.mouseDrag(mouseLoc); } */

/*         // From DrawableContainer */
/*         void draw() override { DrawableContainer::draw(); for (auto &c: *this) c.draw(); } */

/*         // Access */
/*         Cell &at(const Point &p); */
/*         Cell &at(const Point &c, Direction d); */

/*         // Here diagonal neighbours are not considered as neighbours */
/*         bool areNeighbours(const Point &c1, const Point &c2); */
/*         std::vector<Cell*> neighboursOf(const Point& c); */

/*         void select(const Point &c); */
/*         int getSelectedCount() const { return selectedCount; } */
/*         std::vector<Cell*> selected(); */

/*         void clearCell(std::vector<Cell*> &v); */
/*         void clearCell(const Point &point); */
/* }; */

class Grid : public DrawableContainer, public Interactive
{
    public:
        enum class Direction {
            South,
            North,
            West,
            East,
            SouthWest,
            SouthEast,
            NorthWest,
            NorthEast
        };
        constexpr static std::array<Point, 8> directionModifier {
            Point{ 0, -1},  // South
            Point{ 0,  1},  // North
            Point{-1,  0},  // West
            Point{ 1,  0},  // East
            Point{-1, -1},  // SouthWest
            Point{ 1, -1},  // SouthEast
            Point{-1,  1},  // NorthWest
            Point{ 1,  1}   // NorthEast
        };
    protected:
        std::vector<std::vector<std::shared_ptr<Cell>>> matrix;
        int selectedCount = 0;
        bool swapWait = false;  // true if a program is waiting a swap

        // Dimentions
        int cellContentSide = 10;
        int cellSide = 15;
        int colSize;
        int rowSize;

        // Counters for animations that have finished
        unsigned moveQCount = 0;
        unsigned clearQCount = 0;

        // Vectors containing cells to be processed
        // once their animation is finished
        std::vector<Point> moveQueue {};
        std::vector<Point> clearQueue {};
    public:
        Grid(Point center, int width, int height, int rows, int columns);
        Grid(Point center, int width, int height, int side);

        class Iterator {
            private:
                Grid* g; //TODO make shared
                Point coord;
            public:
                Iterator(Grid* g, Point coord) : g{g}, coord{coord} { }

                Iterator &operator++() {
                    int xs = static_cast<int>(g->matrix[0].size());
                    ++coord.x;
                    coord.y += coord.x/xs;
                    coord.x %= xs;
                    return *this;
                }
                Cell &operator*() { return g->at(coord); }
                bool operator!=(const Iterator &other) { return coord != other.coord; }
        };

        Iterator begin() { return Iterator{this, {0, 0}}; }
        Iterator end() { return Iterator{this, {0, static_cast<int>(matrix.size())}}; }

        Cell &at(const Point &p);
        Cell &at(const Point &c, Direction d);

        void select(const Point &c);
        int getSelectedCount() const { return selectedCount; }
        std::vector<Point> getSelected();

        // Here diagonal neighbours are not considered as neighbours
        bool areNeighbours(const Point& c1, const Point& c2);
        std::vector<Point> neighboursOf(const Point& c);

        virtual void draw() override {
            DrawableContainer::draw();
            for (auto &c: *this) c.draw();
            for (auto &c: *this) c.drawContent();
        }

        // Mouse interactions
        void mouseMove(Point mouseLoc) override
        {
            if (!animationPlaying()) for (auto &c: *this) c.mouseMove(mouseLoc);
        }
        void mouseClick(Point mouseLoc) override
        {
            if (!animationPlaying()) for (auto &c: *this) c.mouseClick(mouseLoc);
        }
        void mouseDrag(Point mouseLoc) override
        {
            if (!animationPlaying()) for (auto &c: *this) c.mouseDrag(mouseLoc);
        }

        bool makeFall(const Point &p);

        bool fillGrid();

        std::vector< std::vector<Point> > combinationsFrom(const Point &p);
        bool isInCombination(const Point &point);
        void processCombinationFrom(const Point &point);

        void clearCell(std::vector<Point> &v);
        void clearCell(const Point &point);

        void put(const Point &point, ContentT content, StandardCandy::Color color = StandardCandy::Color::Red, Axis axis = Axis::Vertical);

        // Signals from CellContents
        void clearDone();
        void moveDone();

        bool animationPlaying()
        {
            for (auto &c : *this)
                if (c.hasContentAnimation())
                    return true;
            return false;
        }

        unsigned colCount() { return static_cast<unsigned>(matrix.at(0).size()); }
        unsigned rowCount() { return static_cast<unsigned>(matrix.size()); }
};

#endif

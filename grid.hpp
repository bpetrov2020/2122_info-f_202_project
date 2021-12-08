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
#include "board_state.hpp"

class State;
class ReadyState;
class Grid;
class LevelData;

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
        bool processedThisClearState{ false };
        bool lastSelected = false;

    public:
        Cell(Point center, int width, int height, Point index, Grid &grid);

        // Two functions are needed to have the background
        // and foreground drawn separately
        void draw() override;
        void drawContent();

        void update(Event e);

        // Functions acting on the content of the cell
        bool clear();
        void clearWithoutAnimation();
        void removeContent();

        bool isEmpty() const;
        bool isContentMovable() const;
        /* bool isClearing() { return content->isClearing(); } */
        bool isContentClearing()
        {
            std::shared_ptr<ClearableCellContent> c{std::dynamic_pointer_cast<ClearableCellContent>(content)};
            return c && c->isClearing();
        }

        bool moveContentTo(Cell &other);
        bool swapContentWith(const Point &p);
        void contentWasSwappedWith(const Point &p);
        bool swapContentWithWithoutAnimation(const Point &p);

        auto &getContent() { return content; }
        void setContent(std::shared_ptr<CellContent> c) { content = std::move(c); }

        // actions on lastSelected
        bool isLastSelected() { return lastSelected; }
        void setLastSelected(bool state) { lastSelected = state; }

        // Functions acting on the relations between the cell and its grid
        bool isSelected() const { return selected; }

        const Point getIndex() const { return index; }

        bool toggleSelected();

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        bool hasContentAnimation() { return content ? content->hasAnimation() : false; }

        bool operator==(const Cell &other) { return index == other.index; }

        bool hasMatchWith(const Point &point);

        bool wasProcessedThisClear() const { return processedThisClearState; }
};

class Grid : public DrawableContainer, public Interactive
{
    public:
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
        std::vector<std::vector<std::shared_ptr<Cell>>> matrix {};
        int selectedCount = 0;

        // Dimentions
        int cellContentSide = 10;
        int cellSide = 15;
        int colSize;
        int rowSize;

        std::shared_ptr<State> state;

        int candyColorRange;
    public:
        Grid(Point center, int width, int height, LevelData &data);
        Grid(Point center, int width, int height, int rows, int columns, LevelData &data);

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
        void clearSelection();

        // Here diagonal neighbours are not considered as neighbours
        bool areNeighbours(const Point& c1, const Point& c2);
        std::vector<Point> neighboursOf(const Point& c);

        void draw() override;

        void setState(std::shared_ptr<State> newState)
        {
            state = std::move(newState);
        }

        // Mouse interactions
        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void clearCell(std::vector<Point> &v);
        bool clearCell(const Point &point);
        void clearCellWithoutAnimation(const Point &point)
        {
            at(point).clearWithoutAnimation();
        }

        // For different contents
        void put(const Point &point, ContentT content);  // All no parameter contents
        void put(const Point &point, ContentT content, int layer);  // Icing
        void put(const Point &point, ContentT content, StandardCandy::Color color, Axis axis = std::rand()%2 ? Axis::Horizontal : Axis::Vertical); // Candies

        bool animationPlaying()
        {
            for (auto &c : *this)
                if (c.hasContentAnimation())
                    return true;
            return false;
        }

        unsigned colCount() const { return static_cast<unsigned>(matrix.at(0).size()); }
        unsigned rowCount() const { return static_cast<unsigned>(matrix.size()); }

        bool isIndexValid(const Point &p, Direction d) const;
        bool isIndexValid(const Point &p) const;

        bool swapCellContent(std::vector<Point> toSwap);
        bool swapCellContentWithoutAnimation(std::vector<Point> toSwap);

        void update(Event e);
        void cellContentAnimationFinished(const Point &p);

        int getRowSize() const { return rowSize; }
        int getCellContentSide() const { return cellContentSide; }

        bool isCellEmpty(const Point &p) { return at(p).isEmpty(); }
        bool hasCellMatchWith(const Point &a, const Point &b) { return at(a).hasMatchWith(b); }

        int getCandyColorRange() const { return candyColorRange; }
};

#endif

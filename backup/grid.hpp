#ifndef GRID_HPP
#define GRID_HPP

#include "common.hpp"

#include <vector>

/**
    A grid a cells.

    The cells are accessible and content can be put
    on them.
*/
class Grid : public DrawableContainer<Rectangle>
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
        constexpr static std::array<Point, 8> dirModifier {
            Point{ 0, -1},  // South
            Point{ 0,  1},  // North
            Point{-1,  0},  // West
            Point{ 1,  0},  // East
            Point{-1, -1},  // SouthWest
            Point{ 1, -1},  // SouthEast
            Point{-1,  1},  // NorthWest
            Point{ 1,  1}   // NorthEast
        };
    private:
        std::vector< std::vector<Cell>> matrix;
        int selectionCount = 0;
        bool isExculsive = false;
    public:
        Grid(Point center, int w, int h, std::size_t col, std::size_t row, int gap);

        class Iterator {
            private:
                Grid* g;
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
                bool operator==(const Iterator &other) { return coord == other.coord; }
        };

        iterator begin() { return iterator{this, {0, 0}}; }
        iterator end() { return iterator{this, {0, static_cast<int>(matrix.size())}}; }

        // From Interactive
        void mouseMove(Point mouseLoc) override { for (auto &c: *this) c.mouseMove(mouseLoc); }
        void mouseClick(Point mouseLoc) override { for (auto &c: *this) c.mouseClick(mouseLoc); }
        void mouseDrag(Point mouseLoc) override { for (auto &c: *this) c.mouseDrag(mouseLoc); }

        // From DrawableContainer
        void draw() override { DrawableContainer::draw(); for (auto &c: *this) c.draw(); }

        // Access
        Cell& at(const Point& p)
        {
            return matrix.at(matrix.size()-1-p.y).at(p.x);
        }
        Cell& at(const Point& p, Direction d)
        {
            return at(p+dirModifier[static_cast<std::array::size_type>(d)]);
        }

        // Actions and messages
        void put(const Point& p, std::unique_ptr<Content>& ptr);
        void putDone();

        void clear(const Point& p);
        void clearDone();

        void moveTo(const Point& s, const Point& d);
        void moveDone();

        void swap(const Point& a, const Point& b);
        void swapDone();

        void select(const Point& p);
        int selectedCount() const { return selectedCount; }
        std::vector<Cell*> selected();
};

class Cell : public DrawableContainer
{
    protected:
        std::shared_ptr<CellContent> content;
        Point coordinate;  // position in the grid
        Grid &grid;
        bool selected = false;
    public:
        Cell(Point center, int width, int height, Point coordinate, Grid &grid);

        // Draw the background and the content on it
        virtual void draw() override;

        // Functions acting on the content of the cell
        void clear();
        bool isEmpty() const;
        bool isContentMovable() const;

        void moveContentTo(Cell &other);
        void swapContentWith(Cell &other);

        auto &getContent() { return content; }
        void setContent(std::shared_ptr<CellContent> c) { content = std::move(c); }

        // Functions acting on the relations between the cell and its grid
        bool isSelected() const { return selected; }

        const Point getCoordinate() const { return coordinate; }

        bool toggleSelected();

        void mouseMove(Point mouseLoc);
        void mouseClick(Point mouseLoc);
        void mouseDrag(Point mouseLoc);

        bool operator==(const Cell &other) { return coordinate == other.coordinate; }
};

#endif

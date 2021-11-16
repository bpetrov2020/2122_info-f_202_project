#include "shape.hpp"
#include "point.hpp"
#include "cell_content.hpp"

#include <memory>
#include <vector>
#include <iostream>

template <typename S>
class DrawableObject
{
    protected:
        S drawable;
    public:
        DrawableObject(const S &d) : drawable{d} { }
        virtual ~DrawableObject() = default;

        virtual void draw() { drawable.draw(); }

        Point getCenter() const { return drawable.getCenter(); }
        void setCenter(const Point &p) const { drawable.setCenter(p); }
};

class Grid;

// A Cell and its content should
// have the same center, the only exception
// is in the case of animations.
class Cell : public DrawableObject<Rectangle>
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


class Grid : public DrawableObject<Rectangle>
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
        std::vector< std::vector<Cell> > matrix;
        int selectedCount = 0;
    public:
        Grid(Point center, int width, int height, int rows, int columns);
        Grid(Point center, int width, int height, int side);

        virtual ~Grid() = default;

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

        Iterator begin() { return Iterator{this, {0, 0}}; }
        Iterator end() { return Iterator{this, {0, static_cast<int>(matrix.size())}}; }

        Cell &at(const Point &p);
        Cell &at(const Point &c, Direction d);

        void select(const Point &c);
        int getSelectedCount() const { return selectedCount; }
        std::vector<Cell*> selected();

        // Here diagonal neighbours are not considered as neighbours
        bool areNeighbours(const Cell &c1, const Cell &c2);
        std::vector<Cell*> neighboursOf(const Point& c);

        virtual void draw() override { DrawableObject::draw(); for (auto &c: *this) c.draw(); }

        // Mouse interactions
        void mouseMove(Point mouseLoc)  { for (auto &c: *this) c.mouseMove(mouseLoc); }
        void mouseClick(Point mouseLoc) { for (auto &c: *this) c.mouseClick(mouseLoc); }
        void mouseDrag(Point mouseLoc)  { for (auto &c: *this) c.mouseDrag(mouseLoc); }

        bool makeFall(const Point &p);
        /*         /1* void clearCell(GridCell &c); *1/ */

        /*         /1* void makeFall(GridCell &c); *1/ */

        void fillGrid();

        /* std::vector<Cell*> combinationsFrom(const Point &p); */
        std::vector< std::vector<Cell*> > combinationsFrom(const Point &p);
        bool isInCombination(const Point &point);
        /* void processCombinationFrom(std::vector< std::vector<Cell*>> v); */
        void processCombinationFrom(const Point &point);

        void clearCell(std::vector<Cell*> &v);
        void clearCell(const Point &point);
};

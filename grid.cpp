#include "grid.hpp"

/*----------------------------------------------------------
 *                          Cell
 *--------------------------------------------------------*/

Cell::Cell(Point center, int width, int height, Point coordinate, Grid &grid)
    : DrawableObject<Rectangle>(Rectangle{center, width, height}),
    content{nullptr},
    coordinate{coordinate},
    grid{grid}
{ }

void Cell::draw()
{
    DrawableObject::draw();  // draw itself, i.e. background
    if (content) content->draw();
}

// CONTENT

void Cell::clear()
{
    content.reset();
}

bool Cell::isEmpty() const
{
    return !static_cast<bool>(content);
}

bool Cell::isContentMovable() const
{
    return content->isMovable();
}

void Cell::moveContentTo(Cell &other)
{
    other.clear();  // Warning: whatever was is other cell is destroyed
    content->setCenter(other.getCenter());
    other.content = std::move(content);
}

void Cell::swapContentWith(Cell &other)
{
    content->setCenter(other.getCenter());
    other.content->setCenter(getCenter());
    std::swap(content, other.content);
}

// GRID

bool Cell::toggleSelected()
{
    selected = !selected;
    drawable.setFillColor(selected ? FL_YELLOW : FL_WHITE);
    return selected;
}

void Cell::mouseMove(Point mouseLoc)
{
    ;
}

void Cell::mouseClick(Point mouseLoc)
{
    if (drawable.contains(mouseLoc)) {
        grid.select(coordinate);
    /*     /1* clear(); *1/ */
    /*     /1* grid.makeFall(grid.at(coordinate, Grid::Direction::North).getCoordinate()); *1/ */
    /*     /1* grid.fillGrid(); *1/ */
    }
}

// FIXME : a lot of processing is done here, reduce it
// with maybe a variable counting the selected
void Cell::mouseDrag(Point mouseLoc)
{
    if (grid.getSelectedCount() == 1 && drawable.contains(mouseLoc) && !isSelected())
        for (auto &n: grid.neighboursOf(coordinate))
            if (n->isSelected())
                grid.select(coordinate);
}

/*----------------------------------------------------------
 *                      Grid
 *--------------------------------------------------------*/

Grid::Grid(Point center, int width, int height, int rows, int columns)
    : DrawableObject<Rectangle>(Rectangle{center, width, height, FL_BLACK})
{
    // Down left corner
    Point z = drawable.getCenter() - Point{drawable.getWidth()/2, -drawable.getHeight()/2};

    // Space allocated for each column/row
    int col_s = drawable.getWidth()/columns;
    int row_s = drawable.getHeight()/rows;

    // Between cells
    int space = 10;

    // Dimentions of cells' backgrounds
    int w = col_s - space;
    int h = row_s - space;

    for (int y = 0; y<rows; ++y) {
        matrix.push_back({});
            for (int x = 0; x<columns; ++x)
                matrix[y].push_back(
                        {Point(z.x + (col_s*x + col_s/2),
                               z.y - (row_s*y + row_s/2)),
                        w, h, Point{x, y}, *this});
    }

    for (auto &c: *this) {
        c.setContent(std::shared_ptr<CellContent>(new StandardCandy{c.getCenter(), 40}));
        while (isInCombination(c.getCoordinate())) {
            clearCell(c.getCoordinate());
            c.setContent(std::shared_ptr<CellContent>(new StandardCandy{c.getCenter(), 40}));
        }
    }
}

Grid::Grid(Point center, int width, int height, std::size_t side)
    : Grid(center, width, height, side, side) { }

Cell &Grid::at(const Point &p)
{
    return matrix.at(p.y).at(p.x);
}

Cell &Grid::at(const Point &p, Direction d)
{
    return at(p+directionModifier[static_cast<int>(d)]);
}

void Grid::select(const Point &p)
{
    at(p).toggleSelected() ? ++selectedCount : --selectedCount;
    /* for (auto &i: s) */
    /*     std::cout << i->getCoordinate() << std::endl; */
    if (getSelectedCount()==2) {

        auto selection = selected();

        if (
                ! selection.at(0)->isEmpty()
                && ! selection.at(1)->isEmpty()
                && areNeighbours(*selection.at(0), *selection.at(1))) {

            selection.at(0)->swapContentWith(*selection.at(1));

            bool oneCombination = false;

            for (auto &s: selection) {
                /* std::cout << "in a process\n"; */
                if (isInCombination(s->getCoordinate())) {
                    processCombinationFrom(s->getCoordinate());
                    oneCombination = true;
                }
            }

            /* if (!oneCombination) */
            /*     selection.at(0)->swapContentWith(*selection.at(1)); */
        }

        for (auto &sl: selection) {
            sl->toggleSelected();
            --selectedCount;
        }
    }
}

std::vector<Cell*> Grid::selected()
{
    std::vector<Cell*> ret;
    for (auto &c : *this)
        if (c.isSelected())
            ret.push_back(&c);
    return ret;
}

// Here diagonal neighbours are not considered as neighbours
bool Grid::areNeighbours(const Cell &c1, const Cell &c2)
{
    for (auto &n: neighboursOf(c1.getCoordinate()))
        if (*n == c2)
            return true;
    return false;
    /* Point diff = std::abs(c1-c2); */
    /* return diff == directionModifier[static_cast<int>(Direction::South)] */
    /*     || diff == directionModifier[static_cast<int>(Direction::West)] */
    /*     || diff == directionModifier[static_cast<int>(Direction::North)] */
    /*     || diff == directionModifier[static_cast<int>(Direction::East)]; */
}

std::vector<Cell*> Grid::neighboursOf(const Point& p)
{
    std::vector<Cell*> ret;
    for (int i = 0; i<4; ++i) {
        try {
            Cell &neighbour = at(p+directionModifier[i]);  // Catch coordinates not part of the grid
            ret.push_back(&neighbour);
        } catch (std::out_of_range) { }
    }
    for (auto &a: ret) std::cout << a->getCoordinate();
    return ret;
}

bool Grid::makeFall(const Point &p)
{
    bool hasFallen = false;
    if (!at(p).isEmpty() && at(p).isContentMovable()) {
        try {
            if (at(p, Direction::South).isEmpty()) {
                at(p).moveContentTo(at(p, Direction::South));
                hasFallen = true;
                processCombinationFrom(at(p, Direction::South).getCoordinate());
        }
        /* } else if (!isFillableByFall(cellSouthWestOf(c))) { */
        /*     moveCellContent(c, cellSouthWestOf(c)); */
        /* }  else if (!isFillableByFall(cellSouthEastOf(c))) { */
        /*     moveCellContent(c, cellSouthEastOf(c)); */
        } catch (std::out_of_range) {}
    } else if (at(p).isEmpty() && p.y == matrix.size()-1) {
        at(p).setContent(std::shared_ptr<CellContent>(new StandardCandy{at(p).getCenter(), 40}));
        hasFallen = true;
        processCombinationFrom(p);
    }
    return hasFallen;
}

/*
    Fills the empty cells of the grid with new content
    falling from above.

    Currently the cells can only fall vertically but
    diagonal falling is planned.
*/
void Grid::fillGrid()
{
    bool hasFallen = true;
    while (hasFallen) {
        hasFallen = false;
        for (auto &a: *this)
            hasFallen = hasFallen || makeFall(a.getCoordinate());
    }
}

/* std::vector<Cell*> Grid::combinationFrom(const Point &p, bool Vertical) */
/* { */
/*     if (!at(p).getContent().isMatchable()) */
/*         throw std::invalid_argument{"Grid::combinationFrom -> should be called with matchable arguments"}; */

/*     Point current{p}; */
/*     for (auto &d: {Direction::South, Direction::North}) { */
/*         Point next{current + directionModifier[static_cast<std::size_t(d)]}; */
/*         /1* while ( *1/ */
/*         /1*         at(next).getContent().isMatchable() *1/ */
/*         /1*         && at(current).getContent() *1/ */
/*         /1* while (at(current + directionModifier[static_cast<std::size_t>(d)]).get *1/ */
/*         /1* if () *1/ */
/*     } */
/* } */

/*
    Returns cells which are part of a combination that includes
    the cell passed as argument.

    A combination is a straight sequence of adjacent cells
    having content that is equal.

    E.g: two candies having the same color

    The cell passed as argument is not included in the return
    because she's already known by the caller and he has access
    to it.

    @param cell The cell from which should start combinations
    @return Two vectors of pointers to Cell, the first one for
    cells in vertical combinations, the second one for horizontal ones
*/
std::vector<std::vector<Cell*>> Grid::combinationsFrom(const Point &origin)
{
    std::vector<std::vector<Cell*>> ret {{}, {}};

    for (size_t axis = 0; axis<2; ++axis) {      // Axis : Vertical | Horizontal
        for (size_t card = 0; card<2; ++card) {  // Cardinality : North/South | West/East

            Direction direction{ static_cast<Direction>(2*axis + card) };

            try {
                Cell* curr{ &at(origin) };
                Cell* next{ &at(curr->getCoordinate(), direction) };

                while (!next->isEmpty() && *(curr->getContent()) == *(next->getContent())) {
                    ret[axis].push_back(next);
                    curr = next;
                    next = &at(curr->getCoordinate(), direction);
                }
            } catch (std::out_of_range) { }  // don't do anything, just ignore it
        }
    }

    /* for (auto &d: retVect) { */
    /*     for (auto &s: d) { */
    /*         std::cout << s->getCoordinate() << std::endl; */
    /*     } */
    /* } */

    return ret;
}


/* std::vector<std::vector<Cell*>> Grid::combinationsFrom(Cell &origin) */
/* { */
/*     std::vector<std::vector<Cell*>> ret {{}, {}}; */

/*     for (size_t axis = 0; axis<2; ++axis) {      // Axis : Vertical | Horizontal */
/*         for (size_t card = 0; card<2; ++card) {  // Cardinality : North/South | West/East */

/*             Direction direction{ static_cast<Direction>(2*axis + card) }; */

/*             try { */
/*                 Cell* curr{ &origin }; */
/*                 Cell* next{ &at(*curr, direction) }; */

/*                 while (!*next.isEmpty() && *(curr->getContent()) == *(next->getContent())) { */
/*                     ret[axis].push_back(next); */
/*                     curr = next; */
/*                     next = &at(*curr, direction); */
/*                 } */
/*             } catch (std::out_of_range) { }  // don't do anything, just ignore it */
/*         } */
/*     } */

/*     /1* for (auto &d: retVect) { *1/ */
/*     /1*     for (auto &s: d) { *1/ */
/*     /1*         std::cout << s->getCoordinate() << std::endl; *1/ */
/*     /1*     } *1/ */
/*     /1* } *1/ */

/*     return ret; */
/* } */

// TODO: think about cons and pros of using Cell& vs Point


// Returns whether a given cell is apart of a combination
// of 3 or more cells, vertically or horizontally
bool Grid::isInCombination(const Point &point)
{
    auto combi = combinationsFrom(point);
    return combi.at(0).size() >= 2
        || combi.at(1).size() >= 2;
}

/* void Grid::processCombinationFrom(Cell &c) */
/* { */
/*     auto combi = combinationsFrom(c.getCoordinate()); */

/*     for (auto &dir: combi) {     // Vertical & Horizontal */
/*         switch (dir.size()) { */
/*             case 2: */
/*                 clearCell(dir);  // other cells in combination */
/*                 clearCell(c);    // the origin */
/*                 break; */
/*             case 3: */
/*                 clearCell(dir);  // other cells in combination */
/*                 clearCell(c);    // the origin */
/*                 break; */
/*             case 4: */
/*                 clearCell(dir);  // other cells in combination */
/*                 clearCell(c);    // the origin */
/*                 break; */
/*         } */
/*     } */

/*     fillGrid(); */
/* } */

void Grid::processCombinationFrom(const Point& point)
{
    auto combi = combinationsFrom(point);

    for (auto &dir: combi) {     // Vertical & Horizontal
        switch (dir.size()) {
            case 2:
                clearCell(dir);  // other cells in combination
                clearCell(point);    // the origin
                break;
            case 3:
                clearCell(dir);  // other cells in combination
                clearCell(point);    // the origin
                break;
            case 4:
                clearCell(dir);  // other cells in combination
                clearCell(point);    // the origin
                break;
        }
    }

    fillGrid();
}

// Clear the content of a vector of ptr to Cell
void Grid::clearCell(std::vector<Cell*> &vect)
{
    for (auto &p_cell: vect)
        clearCell(p_cell->getCoordinate());
}

// Clear the content of a Cell
void Grid::clearCell(const Point &point)
{
    at(point).clear();
}

// FIXME: error when switching with a cell near empty ones
//
// NOTE: passing by Point is probably better even if 
// a little cumbersome, because this way methods will only
// work on the matrix, they won't be callable by external actors

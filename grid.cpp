#include "grid.hpp"

/*----------------------------------------------------------
 * Cell
 *--------------------------------------------------------*/

Cell::Cell(Point center, int width, int height, Point index, Grid &grid)
    : DrawableContainer(std::make_shared<Rectangle>(center, width, height, CELL_CLR)),
    grid{grid},
    index{index},
    content{nullptr}
{ }

void Cell::draw()
{
    DrawableContainer::draw();  // draw itself, i.e. background
}

void Cell::drawContent()
{
    if (content) content->draw();
}

// CONTENT

/**
 * @return whether or not the clearing process began
 */
bool Cell::clear()
{
    if (content) {
        std::shared_ptr<ClearableCellContent> c{std::dynamic_pointer_cast<ClearableCellContent>(content)};
        if (c && !c->isClearing()) {
            c->clear();
            return true;
        }
    }
    return false;
}

void Cell::clearWithoutAnimation()
{
    if (content) {
        std::shared_ptr<ClearableCellContent> c{std::dynamic_pointer_cast<ClearableCellContent>(content)};
        if (c && !c->isClearing()) {
            c->clearWithoutAnimation();
        }
    }
}

void Cell::removeContent()
{
    content.reset();
}

bool Cell::isEmpty() const
{
    return !static_cast<bool>(content);
}

bool Cell::isContentMovable() const
{
    return static_cast<bool>(std::dynamic_pointer_cast<MovableCellContent>(content));
}

bool Cell::moveContentTo(Cell &other)
{
    assert(!other.content);  // From game logic perspective
    other.removeContent();  // Warning: whatever was is other cell is destroyed

    std::shared_ptr<MovableCellContent> c{std::dynamic_pointer_cast<MovableCellContent>(content)};
    if (c) {
        c->moveTo(other.getIndex());
        other.content = std::move(content);
        return true;
    }
    return false;
}

bool Cell::swapContentWith(Cell &other)
{
    // Should not be swapping with itself
    assert(getIndex() != other.getIndex());
    assert(getCenter() != other.getCenter());
    assert(content->getCenter() != other.content->getCenter());

    std::shared_ptr<MovableCellContent> c{std::dynamic_pointer_cast<MovableCellContent>(content)};
    std::shared_ptr<MovableCellContent> o{std::dynamic_pointer_cast<MovableCellContent>(other.content)};
    if (c && o) {
        // Add animations
        c->moveTo(other.getIndex());
        o->moveTo(getIndex());

        // Swap pointers
        std::swap(content, other.content);
        return true;
    }
    return false;
}

// GRID

bool Cell::toggleSelected()
{
    selected = !selected;
    drawable->setFillColor(selected ? CELL_SELECT_CLR : CELL_CLR);
    return selected;
}

void Cell::mouseMove(Point mouseLoc)
{
    if (!selected)
        drawable->setFillColor(drawable->contains(mouseLoc) ? CELL_HOVER_CLR : CELL_CLR);
}

void Cell::mouseClick(Point mouseLoc)
{
    if (drawable->contains(mouseLoc)) {
        grid.select(index);
    }
}

void Cell::mouseDrag(Point mouseLoc)
{
    if (grid.getSelectedCount() == 1 && drawable->contains(mouseLoc) && !isSelected())
        for (auto &n: grid.neighboursOf(index))
            if (grid.at(n).isSelected())
                grid.select(index);
}

bool Cell::hasMatchWith(const Point &point)
{
    std::shared_ptr<MatchableCellContent> c{std::dynamic_pointer_cast<MatchableCellContent>(content)};
    /* std::shared_ptr<MatchableCellContent> o{std::dynamic_pointer_cast<MatchableCellContent>(grid.at(point).getContent())}; */
    return c && c->hasMatchWith(point);
}

/*----------------------------------------------------------
 *                      Grid
 *--------------------------------------------------------*/

Grid::Grid(Point center, int width, int height, int rows, int columns)
    : DrawableContainer(std::make_shared<Rectangle>(center, width, height, FL_BLACK)),
    colSize{width/columns},
    rowSize{height/rows}
{
    // Down left corner
    Point z = center - Point{width/2, -height/2};

    // Between cells
    int space = 10; // PUT IN CONSTRUCTOR

    // Dimentions of cells' backgrounds
    int w = colSize - space;
    int h = rowSize - space;

    // Initialization of cell
    for (int y = 0; y<rows; ++y) {
        matrix.push_back({});
            for (int x = 0; x<columns; ++x)
                matrix[y].push_back(
                        std::make_shared<Cell>(
                            Point{z.x + (colSize*x + colSize/2), z.y - (rowSize*y + rowSize/2)},
                            w, h, Point{x, y}, *this
                            )
                        );
    }

    cellContentSide = w>h ? h-20 : w-20; // TODO move to initialization list

    // Put candies, maybe should be in another function ?
    for (auto &c: *this) {
        c.setContent(std::make_shared<StandardCandy>(*this, &c, c.getCenter(), cellContentSide));
        while (isInCombination(c.getIndex())) {
            c.removeContent();
            c.setContent(std::make_shared<StandardCandy>(*this, &c, c.getCenter(), cellContentSide));
        }
    }

    Point point{3, 3};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point={4, 3};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point={2, 2};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {2, 4};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {1, 3};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    //point = {3, 1};
    //put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {4, 1};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {3, 1};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {2, 5};
    put(point, ContentT::Wall);
    point = {3, 5};
    put(point, ContentT::Wall);
    point = {4, 5};
    put(point, ContentT::Wall);
}

Grid::Grid(Point center, int width, int height, int side)
    : Grid(center, width, height, side, side)
{ }

Cell &Grid::at(const Point &p)
{
    return *matrix.at(p.y).at(p.x);
}

Cell &Grid::at(const Point &p, Direction d)
{
    return at(p+directionModifier[static_cast<int>(d)]);
}

void Grid::select(const Point &p)
{
    assert(selectedCount < 2);
    at(p).toggleSelected() ? ++selectedCount : --selectedCount;

    if (getSelectedCount()==2) {
        auto selection = getSelected();

        if (
                ! at(selection.at(0)).isEmpty()
                && ! at(selection.at(1)).isEmpty()
                && areNeighbours(selection.at(0), selection.at(1))
                ) {

            if ( at(selection.at(0)).swapContentWith(at(selection.at(1)))) {
                for (auto &p: selection)
                    moveQueue.push_back(p);

                swapWait = true;
            }
        }
        for (auto &s: selection) {
            at(s).toggleSelected();
            --selectedCount;
        }
    }
}

std::vector<Point> Grid::getSelected()
{
    std::vector<Point> ret;
    for (auto &c : *this)
        if (c.isSelected())
            ret.push_back(c.getIndex());
    return ret;
}

// Here diagonal neighbours are not considered as neighbours
bool Grid::areNeighbours(const Point& p1, const Point& p2)
{
    for (auto &n: neighboursOf(p1))
        if (n == p2)
            return true;
    return false;
}

// TODO make difference between accessible neighbours and
// all neighbours
std::vector<Point> Grid::neighboursOf(const Point& p)
{
    std::vector<Point> ret;
    for (int i = 0; i<4; ++i) {
        try {
            Point idx{p+directionModifier[i]};
            if (isIndexValid(idx)) {
                ret.push_back(idx);
            }
            /* Cell &neighbour = at(p+directionModifier[i]);  // Catch Index not part of the grid */
            /* ret.push_back(neighbour.getIndex()); */
        } catch (const std::out_of_range& err) {std::cout << "Fuck neighboursoF"; }
    }
    return ret;
}

bool Grid::makeFall(const Point &p)
{
    bool hasFallen = false;
    // TODO, clean this mess

    if (!at(p).isEmpty() && at(p).isContentMovable()) {
        try {
            if (isIndexValid(p, Direction::South) && at(p, Direction::South).isEmpty()) {
                /* std::cout << "move " << p << " to " << at(p, Direction::South).getIndex() <<std::endl; */
                if (at(p).moveContentTo(at(p, Direction::South))) {
                    moveQueue.push_back(at(p, Direction::South).getIndex());
                    hasFallen = true;
                }
            } else if (
                    isIndexValid(p, Direction::SouthWest) && at(p, Direction::SouthWest).isEmpty()
                    && (
                        isIndexValid(p, Direction::West) && !at(p, Direction::West).isContentMovable() && !at(p, Direction::West).isEmpty()
                        || (
                            !isFillableByFall(at(p, Direction::SouthWest).getIndex())
                            && !isFillableByFall(p)
                            )
                        )
                    ) {
                if (at(p).moveContentTo(at(p, Direction::SouthWest))) {
                    moveQueue.push_back(at(p, Direction::SouthWest).getIndex());
                    hasFallen = true;
                }
            }  else if (
                    isIndexValid(p, Direction::SouthEast) && at(p, Direction::SouthEast).isEmpty()
                    && (
                        isIndexValid(p, Direction::East) && !at(p, Direction::East).isContentMovable() && !at(p, Direction::East).isEmpty()
                        || (
                            !isFillableByFall(at(p, Direction::SouthEast).getIndex())
                            && !isFillableByFall(p)
                            )
                        )
                    ) {
                if(at(p).moveContentTo(at(p, Direction::SouthEast))) {
                    moveQueue.push_back(at(p, Direction::SouthEast).getIndex());
                    hasFallen = true;
                }
            }
        } catch (const std::out_of_range& err) {std::cout << "Fuck makefall"; }
    }
    if (at(p).isEmpty() && p.y == matrix.size()-1) {
        Cell buffer{at(p).getCenter() - Point{0, rowSize}, 0, 0, {-1, -1}, *this};
        buffer.setContent(std::make_shared<StandardCandy>(*this, &buffer, buffer.getCenter(), cellContentSide));
        buffer.moveContentTo(at(p));
        moveQueue.push_back(p);
        hasFallen = true;
    }
    return hasFallen;
}

/*
    Fills the empty cells of the grid with new content
    falling from above.

    Currently the cells can only fall vertically but
    diagonal falling is planned. TODO
*/
bool Grid::fillGrid()
{
    bool hasFallen = false;
    for (auto &a: *this) {
        hasFallen = makeFall(a.getIndex()) || hasFallen;
    }
    return hasFallen;
}

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
std::vector<std::vector<Point>> Grid::combinationsFrom(const Point& origin)
{
    std::vector<std::vector<Point>> ret {{}, {}};

    for (size_t axis = 0; axis<2; ++axis) {      // Axis : Vertical | Horizontal
        for (size_t card = 0; card<2; ++card) {  // Cardinality : North/South | West/East

            Direction direction{ static_cast<Direction>(2*axis + card) };

            Point curr{origin};

            try {
            while (
                    isIndexValid(curr, direction)
                    && !at(curr, direction).isEmpty()
                    && at(curr).hasMatchWith(at(curr, direction).getIndex())
                    )
            {
                curr = at(curr, direction).getIndex();
                ret[axis].push_back(curr);
            }

            /*     Cell* curr{ &at(origin) }; */
            /*     Cell* next{ &at(curr->getIndex(), direction) }; */

            /*     while (!next->isEmpty() && curr->hasMatchWith(next->getIndex())) { */
            /*         ret[axis].push_back(next->getIndex()); */
            /*         curr = next; */
            /*         next = &at(curr->getIndex(), direction); */
            /*     } */
            } catch (const std::out_of_range& err) {std::cout << "Fuck combinationsFrom";  }  // don't do anything, just ignore it
        }
    }

    return ret;
}


/**
 * Returns whether a given cell is apart of a combination
 * of 3 or more cells, vertically or horizontally
 */
bool Grid::isInCombination(const Point &point)
{
    auto combi = combinationsFrom(point);
    return combi.at(0).size() >= 2
        || combi.at(1).size() >= 2;
}

// TODO trigger clear on FOURTH even when in combination
void Grid::processCombinationFrom(const Point& point)
{
    if (at(point).isContentClearing())
        return ;

    auto combi = combinationsFrom(point);

    const int V = 0;
    const int H = 1;

    unsigned verticalCount = combi.at(0).size();
    unsigned horizontalCount = combi.at(1).size();

    // 3 in one axis
    if ((verticalCount==2 && horizontalCount<2) || (horizontalCount==2 && verticalCount<2)) {
        clearCell(point);
        clearCell(combi.at(horizontalCount>verticalCount ? H : V));

    // 4 in one axis
    } else if ((verticalCount==3 && horizontalCount<2) || (horizontalCount==3 && verticalCount<2)) {
        StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(at(point).getContent())->getColor()};  // TODO simplify this
        at(point).clearWithoutAnimation();
        clearCell(combi.at(horizontalCount>verticalCount ? H : V));
        put(point, ContentT::StripedCandy, color, verticalCount>horizontalCount ? Axis::Vertical : Axis::Horizontal);

    // 5 or more in one axis
    } else if ((verticalCount>=4 && horizontalCount<2) || (horizontalCount>=4 && verticalCount<2)) {
        clearCell(point);
        clearCell(combi.at(horizontalCount>verticalCount ? H : V));

    // More than 3 on both axis
    } else if (verticalCount>=2 && horizontalCount>=2) {
        StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(at(point).getContent())->getColor()};
        at(point).clearWithoutAnimation();
        for (auto &a: combi)
            clearCell(a);
        put(point, ContentT::WrappedCandy, color);
    }

    /* for (auto &dir: combi) {     // Vertical & Horizontal */
    /*     switch (dir.size()) { */
    /*         case 2: */
    /*             clearCell(dir);  // other cells in combination */
    /*             clearCell(point);    // the origin */
    /*             break; */
    /*         case 3: */
    /*             { */
    /*             clearCell(dir);  // other cells in combination */
    /*             /1* clearCell(point);    // the origin *1/ */
    /*             StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(at(point).getContent())->getColor()}; */
    /*             at(point).setContent(std::make_shared<StripedCandy>(*this, &at(point), at(point).getCenter(), cellContentSide, color, verticalCount==3?Axis::Vertical : Axis::Horizontal)); */
    /*             break; */
    /*             } */
    /*         case 4: */
    /*             clearCell(dir);  // other cells in combination */
    /*             clearCell(point);    // the origin */
    /*             break; */
    /*     } */
    /* } */

    /* fillGrid(); */
}

// Clear the content of a vector of ptr to Cell
void Grid::clearCell(std::vector<Point> &vect)
{
    for (auto &p_cell: vect)
        clearCell(p_cell);
}

// Clear the content of a Cell
void Grid::clearCell(const Point &point)
{
    if (dynamic_pointer_cast<WrappedCandy>(at(point).getContent()) != nullptr) {
        at(point).clearWithoutAnimation();
    } else if (at(point).clear()) {
        clearQueue.push_back(point);
    }
}

void Grid::put(const Point &point, ContentT content, StandardCandy::Color color, Axis axis)
{
    std::shared_ptr<CellContent> toPut;

    switch (content) {
        case ContentT::StandardCandy:
            toPut = std::make_shared<StandardCandy>(*this, &at(point), at(point).getCenter(), cellContentSide, color);
            break;
        case ContentT::StripedCandy:
            toPut = std::make_shared<StripedCandy>(*this, &at(point), at(point).getCenter(), cellContentSide, color, axis);
            break;
        case ContentT::WrappedCandy:
            toPut = std::make_shared<WrappedCandy>(*this, &at(point), at(point).getCenter(), cellContentSide, color);
            break;
        case ContentT::WrappedCandySecPhase:
            toPut = std::make_shared<WrappedCandySecPhase>(*this, &at(point), at(point).getCenter(), cellContentSide, color);
            break;
        case ContentT::Wall:
            toPut = std::make_shared<Wall>(*this, &at(point), at(point).getCenter(), cellContentSide);
            break;
    }

    assert(toPut);
    at(point).setContent(toPut);
}

void Grid::clearDone()
{
    assert(clearQCount < clearQueue.size());
    ++clearQCount;
    /* std::cout << "Count : " <<clearQCount << " Queue : " << clearQueue.size() << std::endl; */

    if (clearQCount == clearQueue.size()) {
        // Clearing
        for (auto &i: clearQueue)
            at(i).removeContent();

        // Cleaning
        clearQueue.clear();
        clearQCount = 0;

        // Filling
        fillGrid();
    }
}

// TODO add assertions
void Grid::moveDone()
{
    assert(moveQCount < moveQueue.size());
    ++moveQCount;
    /* std::cout << "Count : " <<moveQCount << " Queue : " << moveQueue.size() << std::endl; */

    if (moveQCount == moveQueue.size()) {
        assert(!animationPlaying());

        if (swapWait) {     // A swap was done
            bool oneCombination = false;

            for (auto &s: moveQueue) {
                if (isInCombination(s)) {
                    processCombinationFrom(s);
                    oneCombination = true;
                }
            }

            if (!oneCombination) {
                at(moveQueue.at(0)).swapContentWith(at(moveQueue.at(1)));
            }

            // Cleaning
            swapWait = false;
            if (oneCombination) moveQueue.clear();
            moveQCount = 0;

        } else {            // A candy has fallen
            bool moreFall = fillGrid();
            if (!moreFall) {
                for (auto &i: moveQueue)
                    processCombinationFrom(i);

                for (auto &c: *this) {      // if we have second phase of wrapped candy, must explode here
                    if (dynamic_pointer_cast<WrappedCandySecPhase>(c.getContent()) != nullptr) {
                        clearCell(c.getIndex());
                    }
                }

                // Cleaning
                moveQueue.clear();
                moveQCount = 0;
            }
        }
    }

}

/**
 * Return whether a cell can be filled by vertical fall
 *
 * Conditions:
 *  - a cell above is not movable;
 *  - cells between the current and the one note movable are empty
 *      and not buffers
 */
//TODO rewrite this mess
bool Grid::isFillableByFall(const Point &point)
{
    bool ret{false};
    Point current{at(point).getIndex()};

    while (current.y<rowCount()) {
        if (!isIndexValid(current, Direction::North))
            break;
        current = at(current, Direction::North).getIndex();
        /* std::cout << current; */
        if (!at(current).isEmpty()) {
            return at(current).isContentMovable();
        }
    }

    return true;
}

bool Grid::isIndexValid(const Point &p, Direction d)
{
    return isIndexValid(p+directionModifier[static_cast<int>(d)]);
}

bool Grid::isIndexValid(const Point &p)
{
    return p.x>=0 && p.x<colCount()
        && p.y>=0 && p.y<rowCount();

}

// NOTE: passing by Point is probably better even if 
// a little cumbersome, because this way methods will only
// work on the matrix, they won't be callable by external actors

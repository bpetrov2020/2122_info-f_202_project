#include "grid.hpp"

/*----------------------------------------------------------
 * Cell
 *--------------------------------------------------------*/

Cell::Cell(Point center, int width, int height, Point index, Grid &grid)
    : DrawableContainer(std::make_shared<Rectangle>(center, width, height)),
    content{nullptr},
    index{index},
    grid{grid}
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

void Cell::clear()
{
    /* content.reset(); */
    content->addAnimation(std::make_shared<ScaleAnimation>(20));
}

void Cell::clearWithoutAnimation()
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
    /* std::cout << "1M\n"; */
    assert(!other.content);
    other.clearWithoutAnimation();  // Warning: whatever was is other cell is destroyed
    /* content->setCenter(other.getCenter()); */
    /* std::cout << "1M\n"; */
    content->moveTo(other.getIndex());
    /* std::cout << "2M\n"; */
    other.content = std::move(content);
    /* std::cout << "3M\n"; */
}

void Cell::swapContentWith(Cell &other)
{
    /* content->setCenter(other.getCenter()); */
    /* other.content->setCenter(getCenter()); */
    /* std::cout << "moving " << content->getCenter() << " to " << other.getCenter() << std::endl; */
    assert(getIndex() != other.getIndex());
    assert(getCenter() != other.getCenter());
    assert(content->getCenter() != other.content->getCenter());

    /* content->moveTo(other.getCenter()); */
    content->moveTo(other.getIndex());
    /* content->moveTo(other); */
    /* other.content->moveTo(*this); */
    /* std::cout << "moving " << other.content->getCenter() << " to " << getCenter() << std::endl; */
    other.content->moveTo(getIndex());
    /* std::cout << "From " << content << " "<<getCenter()<<" to "<<other.getCenter()<<std::endl; */
    /* std::cout << "From " << other.content << " "<<other.getCenter()<<" to "<<getCenter()<<std::endl; */
    std::swap(content, other.content);
    /* std::cout << content << " at " << content->getCenter() << std::endl; */
    /* std::cout << other.content << " at " << other.content->getCenter() << std::endl; */
}

// GRID

bool Cell::toggleSelected()
{
    selected = !selected;
    std::shared_ptr<Rectangle> r{std::dynamic_pointer_cast<Rectangle>(drawable)};  // TODO COLORS IN SHAPE
    r->setFillColor(selected ? FL_YELLOW : FL_WHITE);
    return selected;
}

void Cell::mouseMove(Point mouseLoc)
{
    ;
}

void Cell::mouseClick(Point mouseLoc)
{
    if (drawable->contains(mouseLoc)) {
        grid.select(index);
        /* addAnimation(std::make_shared<ScaleAnimation>(60)); */
        /* content->addAnimation(std::make_shared<ScaleAnimation>(60)); */
    }
}

// FIXME : a lot of processing is done here, reduce it
// with maybe a variable counting the selected
void Cell::mouseDrag(Point mouseLoc)
{
    if (grid.getSelectedCount() == 1 && drawable->contains(mouseLoc) && !isSelected())
        for (auto &n: grid.neighboursOf(index))
            if (grid.at(n).isSelected())
                grid.select(index);
}

/*----------------------------------------------------------
 *                      Grid
 *--------------------------------------------------------*/

Grid::Grid(Point center, int width, int height, int rows, int columns)
    : DrawableContainer(std::make_shared<Rectangle>(center, width, height, FL_BLACK))
{
    // Down left corner
    Point z = center - Point{width/2, -height/2};
    /* std::cout << z; */

    // Space allocated for each column/row
    int col_s = width/columns;
    int row_s = height/rows;
    /* std::cout << col_s << row_s; */

    // Between cells
    int space = 10;

    // Dimentions of cells' backgrounds
    int w = col_s - space;
    int h = row_s - space;

    for (int y = 0; y<rows; ++y) {
        matrix.push_back({});
            for (int x = 0; x<columns; ++x)
                matrix[y].push_back(
                        std::make_shared<Cell>(
                            Point{z.x + (col_s*x + col_s/2), z.y - (row_s*y + row_s/2)},
                            w, h, Point{x, y}, *this
                            )
                        );
    }

    cellContentSide = w>h ? h-20 : w-20;

    for (auto &c: *this) {
        c.setContent(std::make_shared<StandardCandy>(*this, &c, c.getCenter(), cellContentSide));
        while (isInCombination(c.getIndex())) {
            c.clearWithoutAnimation();
            c.setContent(std::make_shared<StandardCandy>(*this, &c, c.getCenter(), cellContentSide));
        }
    }
}

Grid::Grid(Point center, int width, int height, int side)
    : Grid(center, width, height, side, side) { }

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
    at(p).toggleSelected() ? ++selectedCount : --selectedCount;
    /* for (auto &i: s) */
    /*     std::cout << i->getIndex() << std::endl; */
    assert(selectedCount <= 2);
    if (getSelectedCount()==2) {

        auto selection = getSelected();

        if (
                ! at(selection.at(0)).isEmpty()
                && ! at(selection.at(1)).isEmpty()
                && areNeighbours(selection.at(0), selection.at(1))) {

            at(selection.at(0)).swapContentWith(at(selection.at(1)));

            for (auto &p: selection)
                moveQueue.push_back(p);

            swapWait = true;


            /* bool oneCombination = false; */

            /* for (auto &s: selection) { */
            /*     /1* std::cout << "in a process\n"; *1/ */
            /*     if (isInCombination(s->getIndex())) { */
            /*         processCombinationFrom(s->getIndex()); */
            /*         oneCombination = true; */
            /*     } */
            /* } */

            /* if (!oneCombination) */
            /*     selection.at(0)->swapContentWith(*selection.at(1)); */
        }

        for (auto &sl: selection) {
            std::cout << sl << std::endl;
            at(sl).toggleSelected();
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
    /* Point diff = std::abs(c1-c2); */
    /* return diff == directionModifier[static_cast<int>(Direction::South)] */
    /*     || diff == directionModifier[static_cast<int>(Direction::West)] */
    /*     || diff == directionModifier[static_cast<int>(Direction::North)] */
    /*     || diff == directionModifier[static_cast<int>(Direction::East)]; */
}

std::vector<Point> Grid::neighboursOf(const Point& p)
{
    std::vector<Point> ret;
    for (int i = 0; i<4; ++i) {
        try {
            Cell &neighbour = at(p+directionModifier[i]);  // Catch Index not part of the grid
            ret.push_back(neighbour.getIndex());
        } catch (const std::out_of_range& err) { }
    }
    /* for (auto &a: ret) std::cout << a->getIndex(); */
    return ret;
}

bool Grid::makeFall(const Point &p)
{
    /* std::cout << "1\n"; */
    bool hasFallen = false;
    if (!at(p).isEmpty() && at(p).isContentMovable()) {
        /* std::cout << "2\n"; */
        try {
            if (at(p, Direction::South).isEmpty()) {
                /* std::cout << "3\n"; */
                at(p).moveContentTo(at(p, Direction::South));
                moveQueue.push_back(p-directionModifier.at(static_cast<int>(Direction::South)));
                /* std::cout << "pushed " << p-directionModifier.at(static_cast<int>(Direction::South))<< std::endl; */
                /* std::cout << "4\n"; */
                hasFallen = true;
                if (p.y == matrix.size()-1) {
                    Cell buffer{at(p).getCenter()-Point{0, cellContentSide}, cellContentSide, cellContentSide, {-1, -1}, *this};
                    buffer.setContent(std::make_shared<StandardCandy>(*this, &buffer, buffer.getCenter(), cellContentSide));
                    buffer.moveContentTo(at(p));
                    moveQueue.push_back(p);
                    /* std::cout << "pushed " << p<< std::endl; */
                    hasFallen = true;
                }
                /* processCombinationFrom(at(p, Direction::South).getIndex()); */
        }
        /* } else if (!isFillableByFall(cellSouthWestOf(c))) { */
        /*     moveCellContent(c, cellSouthWestOf(c)); */
        /* }  else if (!isFillableByFall(cellSouthEastOf(c))) { */
        /*     moveCellContent(c, cellSouthEastOf(c)); */
        } catch (const std::out_of_range& err) {}
    } else if (at(p).isEmpty() && p.y == matrix.size()-1) {
        /* std::cout << "5\n"; */
        /* assert(!this->animationPlaying()); */
        Cell buffer{at(p).getCenter()+Point{0, -60}, cellContentSide, cellContentSide, {-1, -1}, *this};
        buffer.setContent(std::make_shared<StandardCandy>(*this, &buffer, buffer.getCenter(), cellContentSide));
        buffer.moveContentTo(at(p));
        moveQueue.push_back(p);
        /* std::cout << "pushed " << p<< std::endl; */
        hasFallen = true;
        /* processCombinationFrom(p); */
    }
    return hasFallen;
}

/*
    Fills the empty cells of the grid with new content
    falling from above.

    Currently the cells can only fall vertically but
    diagonal falling is planned.
*/
bool Grid::fillGrid()
{
    /* bool hasFallen = true; */
    /* while (hasFallen) { */
    /*     hasFallen = false; */
    /*     for (auto &a: *this) */
    /*         hasFallen = hasFallen || makeFall(a.getIndex()); */
    /* } */
    bool hasFallen = false;
    /* std::cout << "BEF\n"; */
    for (auto &a: *this) {
        /* std::cout << "ONE"; */
        hasFallen = makeFall(a.getIndex()) || hasFallen;
    }
    /* for (int i=0; i<9; ++i) */
    /*     for (int j=0; j<9; ++j) */
    /*         makeFall(Point{i, j}); */
    /* std::cout << "AFT\n"; */
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

            try {
                Cell* curr{ &at(origin) };
                Cell* next{ &at(curr->getIndex(), direction) };

                while (!next->isEmpty() && *(curr->getContent()) == *(next->getContent())) {
                    ret[axis].push_back(next->getIndex());
                    curr = next;
                    next = &at(curr->getIndex(), direction);
                }
            } catch (const std::out_of_range& err) { }  // don't do anything, just ignore it
        }
    }

    /* for (auto &d: retVect) { */
    /*     for (auto &s: d) { */
    /*         std::cout << s->getIndex() << std::endl; */
    /*     } */
    /* } */

    return ret;
}


// Returns whether a given cell is apart of a combination
// of 3 or more cells, vertically or horizontally
bool Grid::isInCombination(const Point &point)
{
    auto combi = combinationsFrom(point);
    return combi.at(0).size() >= 2
        || combi.at(1).size() >= 2;
}

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
    if (!at(point).isClearing()) {
        at(point).clear();
        clearQueue.push_back(point);
    }
    /* std::cout << "pushed " << point<< std::endl; */
}

void Grid::clearDone()
{
    assert(clearQCount < clearQueue.size());
    ++clearQCount;
    /* std::cout << "Count : " <<clearQCount << " Queue : " << clearQueue.size() << std::endl; */

    if (clearQCount == clearQueue.size()) {
        // Clearing
        for (auto &i: clearQueue)
            at(i).clearWithoutAnimation();

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
            assert(!animationPlaying());

            bool moreFall = fillGrid();
            if (!moreFall) {
                for (auto &i: moveQueue)
                    if (isInCombination(i))
                        processCombinationFrom(i);

                // Cleaning
                moveQueue.clear();
                moveQCount = 0;
            }
        }
    }

}

// FIXME: error when switching with a cell near empty ones
//
// NOTE: passing by Point is probably better even if 
// a little cumbersome, because this way methods will only
// work on the matrix, they won't be callable by external actors

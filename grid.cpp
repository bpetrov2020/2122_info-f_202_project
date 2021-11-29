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

void Cell::update(Event e)
{
    /* switch (e) { */
    /*     case Event::cellContentClearFinished: */
    /*         removeContent(); */
    /*         grid.update(cellContentAnimationFinished); */
    /*         break; */
    /* } */
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
            removeContent();
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

bool Cell::swapContentWith(const Point &p)
{
    Cell &other {grid.at(p)};
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


bool Cell::contentWasSwappedWith(const Point &p)
{
    assert(content);
    std::shared_ptr<MovableCellContent> movContent {std::dynamic_pointer_cast<MovableCellContent>(content)};
    movContent->wasSwappedWith(p);
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

Grid::Grid(Point center, int width, int height, int side)
    : Grid(center, width, height, side, side)
{ }

Grid::Grid(Point center, int width, int height, int rows, int columns)
    : DrawableContainer(std::make_shared<Rectangle>(center, width, height, FL_BLACK)),
    colSize{width/columns},
    rowSize{height/rows},
    state{std::make_shared<ReadyState>(*this)}
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
    point={2, 1};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point={2, 2};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {2, 4};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {1, 3};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {3, 1};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {4, 1};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {5, 1};
    put(point, ContentT::StandardCandy, StandardCandy::Color::Red);
    point = {2, 5};
    put(point, ContentT::Wall);
    point = {3, 5};
    put(point, ContentT::Wall);
    point = {4, 5};
    put(point, ContentT::Wall);
}

void Grid::mouseMove(Point mouseLoc)
{
    state->mouseMove(mouseLoc);
}

void Grid::mouseClick(Point mouseLoc)
{
    state->mouseClick(mouseLoc);
}

void Grid::mouseDrag(Point mouseLoc)
{
    state->mouseDrag(mouseLoc);
}

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
    state->update(Event::gridSelectionChanged);
}

std::vector<Point> Grid::getSelected()
{
    std::vector<Point> ret;
    for (auto &c : *this)
        if (c.isSelected())
            ret.push_back(c.getIndex());
    return ret;
}

void Grid::clearSelection()
{
    for (auto &c : *this)
        if (c.isSelected()) {
            c.toggleSelected();
            --selectedCount;
        }
    assert(selectedCount == 0);
}

/**
 * Diagonal neighbours are not considered as neighbours
 */
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
        Point idx{p+directionModifier[i]};
        if (isIndexValid(idx)) {
            ret.push_back(idx);
        }
    }
    return ret;
}

void Grid::update(Event e)
{
    /* switch (e) { */
    /*     case Event::cellContentAnimationFinished: */
    /*         state->update(Event::cellContentAnimationFinished); */
    /*         break; */
    /* } */
}

// TODO replace with update
void Grid::cellContentAnimationFinished(const Point &p)
{
    state->animationFinished(p);
}

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

// Clear the content of a vector of ptr to Cell
void Grid::clearCell(std::vector<Point> &vect)
{
    for (auto &p_cell: vect)
        clearCell(p_cell);
}

// Clear the content of a Cell
bool Grid::clearCell(const Point &point)
{
    /* EventManager::get().send(Event::CellCleared); */
    return at(point).clear();
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
        case ContentT::Wall:
            toPut = std::make_shared<Wall>(*this, &at(point), at(point).getCenter(), cellContentSide);
            break;
    }

    assert(toPut);
    at(point).setContent(toPut);
}

bool Grid::isIndexValid(const Point &p, Direction d) const
{
    return isIndexValid(p+directionModifier[static_cast<int>(d)]);
}

bool Grid::isIndexValid(const Point &p) const
{
    return p.x>=0 && p.x<colCount()
        && p.y>=0 && p.y<rowCount();
}

bool Grid::swapCellContent(std::vector<Point> toSwap)
{
    assert(toSwap.size() == 2);
    return at(toSwap.at(0)).swapContentWith(toSwap.at(1));
}

// NOTE: passing by Point is probably better even if 
// a little cumbersome, because this way methods will only
// work on the matrix, they won't be callable by external actors

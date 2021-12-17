#include "grid.hpp"
#include "game.hpp"

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
    if (!isEmpty())
        content->draw();
}

void Cell::update(Event e)
{
    switch (e) {
        case Event::FallStateEnd:
            processedThisClearState = false;
            if (!isEmpty())
                content->update(e);
            break;
        case Event::PulseAnimationFinished:
            grid.update(Event::HintAnimationFinished);
        default:
            break;

        /* case Event::cellContentClearFinished: */
        /*     removeContent(); */
        /*     grid.update(cellContentAnimationFinished); */
        /*     break; */
    }
}

// CONTENT

/**
 * @return whether or not the clearing process began
 */
bool Cell::clear()
{
    processedThisClearState = true;
    if (!isEmpty()) {
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
    processedThisClearState = true;
    if (!isEmpty()) {
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
        swapContentWithWithoutAnimation(p);
        return true;
    }
    return false;
}

bool Cell::swapContentWithWithoutAnimation(const Point &p)
{
    Cell &otherCell {grid.at(p)};

    std::shared_ptr<MovableCellContent> curr{std::dynamic_pointer_cast<MovableCellContent>(content)};
    std::shared_ptr<MovableCellContent> other{std::dynamic_pointer_cast<MovableCellContent>(otherCell.getContent())};

    if (curr && other) {
        content->setCenter(otherCell.getCenter());
        other->setCenter(getCenter());
        std::swap(content, otherCell.content);
        return true;
    }
    return false;
}

void Cell::contentWasSwappedWith(const Point &p)
{
    assert(!isEmpty());
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

bool Cell::hint()
{
    assert(!isEmpty() && !hasContentAnimation());

    content->addAnimation(std::make_shared<PulseAnimation>(20));

    return true;
}

/*----------------------------------------------------------
 *                      Grid
 *--------------------------------------------------------*/

Grid::Grid(Point center, int width, int height, LevelData &data)
    : Grid(center, width, height, data.getGridSize(), data.getGridSize(), data)
{ }

Grid::Grid(Point center, int width, int height, int rows, int columns, LevelData &data)
    : DrawableContainer(std::make_shared<Rectangle>(center, width, height, FL_BLACK)),
    colSize{width/columns},
    rowSize{height/rows},
    state{nullptr},
    candyColorRange{data.getColorRange()}
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
                matrix[static_cast<unsigned>(y)].push_back(
                        std::make_shared<Cell>(
                            Point{z.x + (colSize*x + colSize/2), z.y - (rowSize*y + rowSize/2)},
                            w, h, Point{x, y}, *this
                            )
                        );
    }

    cellContentSide = w>h ? h-20 : w-20; // TODO move to initialization list

    /* setState(std::make_shared<ReadyState>(*this, true, data)); */
    /* setState(std::make_shared<GridInitState>(*this, data)); */
    /* setState(std::make_shared<MessageShower>(*this, "Start")); */
    /* setState(std::make_shared<EditState>(*this)); */
}

void Grid::draw() {
    DrawableContainer::draw();
    for (auto &c: *this) c.draw();
    for (auto &c: *this) c.drawContent();
}

void Grid::mouseMove(Point mouseLoc)
{
    for (auto &c: *this)
        c.mouseMove(mouseLoc);
}

void Grid::mouseClick(Point mouseLoc)
{
    for (auto &c: *this)
        c.mouseClick(mouseLoc);
}

void Grid::mouseDrag(Point mouseLoc)
{
    for (auto &c: *this)
        c.mouseDrag(mouseLoc);
}

Cell &Grid::at(const Point &p)
{
    return *matrix.at(static_cast<unsigned>(p.y)).at(static_cast<unsigned>(p.x));
}

Cell &Grid::at(const Point &p, Direction d)
{
    return at(p+directionModifier[static_cast<unsigned>(d)]);
}

void Grid::select(const Point &p)
{
    at(p).toggleSelected() ? ++selectedCount : --selectedCount;
    if (selectedCount == 2) { at(p).setLastSelected(true); }
    state->update(Event::SelectionChanged);
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
        Point idx{p+directionModifier[static_cast<unsigned>(i)]};
        if (isIndexValid(idx)) {
            ret.push_back(idx);
        }
    }
    return ret;
}

void Grid::update(Event event)
{
    state->update(event);
    /* switch (e) { */
    /*     case Event::cellContentAnimationFinished: */
    /*         state->update(Event::cellContentAnimationFinished); */
    /*         break; */
    /* } */
}

// TODO replace with update
void Grid::cellContentAnimationFinished(const Point &p)
{
    state->gridAnimationFinished(p);
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
    bool clearedCell {at(point).clear()};
    if (clearedCell)
        state->update(Event::CellCleared);
    return clearedCell;
}

void Grid::put(const Point &point, ContentT content)
{
    std::shared_ptr<CellContent> toPut;

    switch (content) {
        case ContentT::StandardCandy:
            toPut = std::make_shared<StandardCandy>(*this, &at(point), at(point).getCenter(), cellContentSide, static_cast<StandardCandy::Color>(std::rand()%getCandyColorRange()));
            break;
        case ContentT::Wall:
            toPut = std::make_shared<Wall>(*this, &at(point), at(point).getCenter(), cellContentSide);
            break;
        case ContentT::ColourBomb:
            toPut = std::make_shared<ColourBomb>(*this, &at(point), at(point).getCenter(), cellContentSide);
            break;
        default:
            break;
    }

    assert(toPut);
    at(point).setContent(toPut);
}

void Grid::put(const Point &point, ContentT content, int layer)
{
    std::shared_ptr<CellContent> toPut;

    switch (content) {
        case ContentT::Icing:
            toPut = std::make_shared<Icing>(*this, &at(point), at(point).getCenter(), cellContentSide, layer);
            break;
        default:
            break;
    }

    assert(toPut);
    at(point).setContent(toPut);
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
        case ContentT::Icing:
            toPut = std::make_shared<Icing>(*this, &at(point), at(point).getCenter(), cellContentSide);
            break;
        default:
            break;
    }

    assert(toPut);
    at(point).setContent(toPut);
}

bool Grid::isIndexValid(const Point &p, Direction d) const
{
    return isIndexValid(p+directionModifier[static_cast<unsigned>(d)]);
}

bool Grid::isIndexValid(const Point &p) const
{
    return p.x>=0 && static_cast<unsigned>(p.x)<colCount()
        && p.y>=0 && static_cast<unsigned>(p.y)<rowCount();
}

bool Grid::swapCellContent(std::vector<Point> toSwap)
{
    assert(toSwap.size() == 2);
    return at(toSwap.at(0)).swapContentWith(toSwap.at(1));
}

bool Grid::swapCellContentWithoutAnimation(std::vector<Point> toSwap)
{
    assert(toSwap.size() == 2);
    return at(toSwap.at(0)).swapContentWithWithoutAnimation(toSwap.at(1));
}

bool Grid::hint(Point p)
{
    return at(p).hint();
}

void Grid::removeAnimations()
{
    for (auto &c: *this)
        c.removeContentAnimation();
}

// NOTE: passing by Point is probably better even if 
// a little cumbersome, because this way methods will only
// work on the matrix, they won't be callable by external actors

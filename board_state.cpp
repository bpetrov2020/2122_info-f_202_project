#include "board_state.hpp"
#include "game.hpp"

/*----------------------------------------------------------
 * Combination
 *--------------------------------------------------------*/

Combination::Combination(const Point &point)
    : origin{point}
{ }

auto Combination::getOrigin() const
{
    return origin;
}

void Combination::addVerticalElement(const Point &elem)
{
    assert(elem.x == origin.x);
    vertical.push_back(elem);
}

void Combination::addHorizontalElement(const Point &elem)
{
    assert(elem.y == origin.y);
    horizontal.push_back(elem);
}

void Combination::addElement(const Point &elem, Direction direction)
{
    switch (direction) {
        case Direction::South:
        case Direction::North:
            addVerticalElement(elem);
            break;
        case Direction::West:
        case Direction::East:
            addHorizontalElement(elem);
            break;
        default:
            std::runtime_error("Combination: given direction is unsupported");
    }
}

std::size_t Combination::getVerticalCount() const
{
    return vertical.size()+1;
}

std::size_t Combination::getHorizontalCount() const
{
    return horizontal.size()+1;
}

std::size_t Combination::getTotalCount() const
{
    std::size_t ret{ 0 };
    ret += getHorizontalCount()>2 ? getHorizontalCount() : 0;
    ret += getVerticalCount()>2 ? getVerticalCount() : 0;
    return ret;
}

auto Combination::getVerticalElements()
{
    return vertical;
}

auto Combination::getHorizontalElements()
{
    return horizontal;
}

auto Combination::getAllElements()
{
    std::vector<Point> ret;
    ret.reserve(vertical.size() + horizontal.size());
    ret.insert(ret.end(), vertical.begin(), vertical.end());
    ret.insert(ret.end(), horizontal.begin(), horizontal.end());
    return ret;
}


/*----------------------------------------------------------
 * State
 *--------------------------------------------------------*/

bool State::isWaiting() const
{
    return grid.animationPlaying();
}

void State::notifyCells(Event e)
{
    for (auto &c: grid) {
        c.update(e);
    }
}

void State::update(Event event)
{
    level.update(event);
    switch (event) {
        case Event::CellCleared:
            level.updateScore(50);
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------
 * MessageState
 *--------------------------------------------------------*/

MessageState::MessageState(Level &level, Grid &grid, std::string msg, int duration) noexcept
    :
        State{level, grid},
        DrawableContainer{std::make_shared<Rectangle>(grid.getCenter(), level.w(), 50, FL_WHITE)},
        message{grid.getCenter(), msg, 14}
{
    addAnimation(std::make_shared<StillAnimation>(duration));
}

void MessageState::draw()
{
    DrawableContainer::draw();
    message.draw();

    // This is dirty, but a way to know when the animation is
    // done and any action can be done. Sometimes changing state
    // in animationFinished could make the program call pure
    // virtual functions, something that doesn't happen here.
    if (messageFinished)
        onTimeout();
}

void MessageState::animationFinished(AnimationT animationType)
{
    switch (animationType) {
        case AnimationT::StillAnimation:
            messageFinished = true;
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------
 * NoActionState
 *--------------------------------------------------------*/

NoActionState::NoActionState(Level &level, Grid &grid) noexcept
    :
        MessageState{level, grid, "No more combinations. Changing grid.", 60}
{ }

void NoActionState::onTimeout()
{
    level.setState(std::make_shared<ReadyState>(level, grid, true));
}

/*----------------------------------------------------------
 * LevelPassedState
 *--------------------------------------------------------*/

LevelPassedState::LevelPassedState(Level &level, Grid &grid) noexcept
    :
        MessageState{level, grid, "You won! Loading next level...", 120}
{ }

void LevelPassedState::onTimeout()
{
    level.replayLevel();
}

/*----------------------------------------------------------
 * LevelNotPassedState
 *--------------------------------------------------------*/

LevelNotPassedState::LevelNotPassedState(Level &level, Grid &grid) noexcept
    :
        MessageState{level, grid, "Thou lost... Try again!", 120}
{ }

void LevelNotPassedState::onTimeout()
{
    level.replayLevel();
}

/*----------------------------------------------------------
 * EditState
 *--------------------------------------------------------*/

void EditState::selectionChanged()
{
    assert(grid.getSelectedCount() == 1);

    auto selection = grid.getSelected();
    grid.clearSelection();

    /* grid.put(selection.at(0), ContentT::StandardCandy, StandardCandy::Color::Red); */
    grid.put(selection.at(0), ContentT::Wall);
}

void EditState::mouseMove(Point mouseLoc)
{
    for (auto &c: grid)
        c.mouseMove(mouseLoc);
}

void EditState::mouseClick(Point mouseLoc)
{
    for (auto &c: grid)
        c.mouseClick(mouseLoc);
}

void EditState::mouseDrag(Point mouseLoc)
{
    for (auto &c: grid)
        c.mouseDrag(mouseLoc);
}

/*----------------------------------------------------------
 * MatchState
 *--------------------------------------------------------*/

bool MatchState::processCombinationContaining(const Point &elem)
{
    if (grid.at(elem).isContentClearing() || grid.at(elem).wasProcessedThisClear())
        return false;

    auto combi {getCombinationContaining(elem)};
    bool oneCombination {false};

    Point origin {combi.getOrigin()};
    std::size_t vc = combi.getVerticalCount();
    std::size_t hc = combi.getHorizontalCount();

    auto largestDirection {hc>vc ? combi.getHorizontalElements() : combi.getVerticalElements()};
    auto smallestDirection {hc<vc ? combi.getHorizontalElements() : combi.getVerticalElements()};

    // 3 in one axis
    if ((vc==3 && hc<3) || (hc==3 && vc<3)) {
        grid.clearCell(origin);
        grid.clearCell(largestDirection);
        oneCombination = true;
        level.updateScore(50);

    // 4 in one axis
    } else if ((vc==4 && hc<3) || (hc==4 && vc<3)) {
        StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(grid.at(origin).getContent())->getColor()};  // TODO simplify this

        grid.clearCellWithoutAnimation(origin);
        grid.clearCell(largestDirection);
        grid.put(origin, ContentT::StripedCandy, color, vc<hc ? Axis::Vertical : Axis::Horizontal);
        oneCombination = true;
        level.updateScore(125);

    // More than 3 on both axis
    } else if (vc>=3 && vc<5 && hc>=3 && hc<5) {
        StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(grid.at(origin).getContent())->getColor()};
        grid.clearCellWithoutAnimation(origin);
        for (auto &a: combi.getAllElements()) {
            grid.clearCell(a);
        }
        grid.put(origin, ContentT::WrappedCandy, color);
        oneCombination = true;
        level.updateScore(200);

    // 5 or more in one axis
    } else if (vc>=5 || hc>=5) {
        grid.clearCellWithoutAnimation(origin);
        grid.clearCell(largestDirection);
        grid.put(origin, ContentT::ColourBomb);
        oneCombination = true;
        level.updateScore(500);
    }


    if (oneCombination) {
        auto vec{ combi.getAllElements() };
        vec.push_back(origin);

        for (auto &c: vec)
            for (auto &n: grid.neighboursOf(c))
                if (!grid.at(n).isEmpty())
                    grid.at(n).getContent()->update(Event::NeighbourMatched);
    }

    return oneCombination;
}

/**
 * Returns cells which are part of a combination that includes
 * the cell passed as argument.
 *
 * A combination is a straight sequence of adjacent cells
 * having content that is equal.
 *
 * E.g: two candies having the same color
 *
 * The cell passed as argument is not included in the return
 * because she's already known by the caller and he has access
 * to it.
 *
 * @param cell The cell from which should start combinations
 * @return Two vectors of pointers to Cell, the first one for
 * cells in vertical combinations, the second one for horizontal ones
 */
Combination MatchState::getCombinationContaining(const Point &origin, bool rec)
{
    assert(!grid.at(origin).isContentClearing());
    Combination ret{origin};

    // Gather combination having origin as starting point
    for (auto &direction: {Direction::East, Direction::North, Direction::West, Direction::South}) {
        Point curr{origin};
        /* Point next{origin, direction}; TODO */
        Point next{origin + Grid::directionModifier[static_cast<unsigned>(direction)]};

        while (grid.isIndexValid(next)
                && !grid.isCellEmpty(next)
                && grid.hasCellMatchWith(curr, next)
                && !grid.at(next).isContentClearing())
        {
            curr = next;
            ret.addElement(curr, direction);
            next = Point{curr + Grid::directionModifier[static_cast<unsigned>(direction)]};
        }
    }

    // Check wether it's the best combination containing origin
    if (rec) {
        for (auto &elem: ret.getAllElements()) {
            auto tmp {getCombinationContaining(elem, false)};
            if (tmp.getTotalCount() > ret.getTotalCount()) {
                ret = tmp;
            }
        }
    }

    return ret;
}

/**
 * Returns whether a given cell is apart of a combination
 * of 3 or more cells, vertically or horizontally
 */
bool MatchState::isInCombination(const Point &point)
{
    auto combi = getCombinationContaining(point);
    return combi.getVerticalCount() >= 3
        || combi.getHorizontalCount() >= 3;
}

/*----------------------------------------------------------
 * GridInitState
 *--------------------------------------------------------*/

GridInitState::GridInitState(Level &level, Grid &grid, LevelData &data)
    : MatchState{level, grid}
{
    putInitialContent(data);
    fillEmptyCells();
}

void GridInitState::draw()
{
    level.setState(std::make_shared<ReadyState>(level, grid));
}

void GridInitState::putInitialContent(LevelData &data)
{
    for (auto &pos: data.getWallsPos())
        grid.put(pos, ContentT::Wall);
    for (auto &pos: data.getSingleIncingPos())
        grid.put(pos, ContentT::Icing, 1);
    for (auto &pos: data.getDoubleIcingPos())
        grid.put(pos, ContentT::Icing, 2);

    // bunch of blue candies for damn bug
    /*Point point = {1,1};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);
    point = {2,0};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);
    point = {2,1};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);
    point = {2,2};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);
    point = {3,2};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);
    point = {4,2};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);*/

    // Double striped
    Point point{1, 1};
    grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue);
    point = {2, 1};
    grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue, Axis::Vertical);

    // Striped with Wrapped
    point = {4, 3};
    grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);
    point = {5, 3};
    grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue, Axis::Vertical);

    // Double wrapped
    /*Point point{1, 1};
    grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);
    point = {2, 1};
    grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);*/

    // ColourBomb with Standard
    /*Point point{1, 1};*/
    point = {1, 1};
    grid.put(point, ContentT::ColourBomb);
    /* point = {1, 2};*/
    /* grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);*/

    /* point = {4, 4};*/
    /* grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);*/
    /* point = {0, 5};*/
    /* grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue, Axis::Horizontal);*/

    // ColourBomb with Wrapped
    /*Point point{1, 1};
    grid.put(point, ContentT::ColourBomb);
    point = {1, 2};
    grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);*/

    // ColourBomb with Striped
    /*Point point{1, 1};
    grid.put(point, ContentT::ColourBomb);
    point = {1, 2};
    grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue);*/

    // Striped making ColourBomb explode
    /*Point point{1, 1};
    grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Red);
    point = {3, 1};
    grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue, Axis::Horizontal);
    point = {4, 2};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);
    point = {5, 1};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);*/

    // Two ColourBombs
    /*Point point{1, 1};
    grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Red);
    point = {1, 2};
    grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Blue);*/

    // add an icing
    /*point = {5, 4};
    grid.put(point, ContentT::Icing, StandardCandy::Color::Blue);*/
}

void GridInitState::fillEmptyCells()
{
    for (auto &c: grid) {
        if (c.isEmpty()) {
            grid.put(c.getIndex(), ContentT::StandardCandy);
            while (isInCombination(c.getIndex())) {
                c.removeContent();
                grid.put(c.getIndex(), ContentT::StandardCandy);
            }
        }
    }
}

/*----------------------------------------------------------
 * ReadyState
 *--------------------------------------------------------*/

ReadyState::ReadyState(Level &level, Grid &grid, bool replaceGrid_) noexcept
    : MatchState{level, grid}
{
    std::cout << "Entering Ready state" << std::endl;
    if (replaceGrid_)
        while (!isActionPossible())
            replaceGrid();
    hasPossibleAction = isActionPossible();
    std::cout << (hasPossibleAction ? "More action" : "No more action") << std::endl;
}

void ReadyState::draw()
{
    if (!hasPossibleAction)
        level.setState(std::make_shared<NoActionState>(level, grid));
}

// TODO
void ReadyState::replaceGrid()
{
    for (auto &c: grid) {
        if (!c.isEmpty() && c.getContent()->getType() == ContentT::StandardCandy) {
            grid.put(c.getIndex(), ContentT::StandardCandy);
            while (isInCombination(c.getIndex())) {
                c.removeContent();
                grid.put(c.getIndex(), ContentT::StandardCandy);
            }
        }
    }
}

bool ReadyState::isActionPossible()
{
    bool actionPossible{false};

    for (auto &c: grid) {
        for (auto &d: {Direction::North, Direction::East}) {
            if (grid.isIndexValid(c.getIndex(), d)) {
                std::vector<Point> toSwap {c.getIndex(), grid.at(c.getIndex(), d).getIndex()};
                grid.swapCellContentWithoutAnimation(toSwap);
                for (auto &p: toSwap)
                    actionPossible = actionPossible || isInCombination(p);
                grid.swapCellContentWithoutAnimation(toSwap);
            }
        }
        if (actionPossible)
            break;
    }

    return actionPossible;
}

void ReadyState::gridAnimationFinished(const Point &)
{
    throw std::runtime_error("There should be no animations in ReadyState");
}

void ReadyState::mouseMove(Point mouseLoc)
{
    grid.mouseMove(mouseLoc);
}

void ReadyState::mouseClick(Point mouseLoc)
{
    grid.mouseClick(mouseLoc);
}

void ReadyState::mouseDrag(Point mouseLoc)
{
    grid.mouseDrag(mouseLoc);
}

void ReadyState::selectionChanged()
{
    if (grid.getSelectedCount() == 2) {

        auto selection = grid.getSelected();
        grid.clearSelection();

        if (
                !grid.at(selection.at(0)).isEmpty()
                && !grid.at(selection.at(1)).isEmpty()
                && grid.areNeighbours(selection.at(0), selection.at(1))
                && grid.swapCellContent(selection)
           ) {
            level.setState(std::make_shared<SwapState>(level, grid));
        }
    }
}

/*----------------------------------------------------------
 * FallState
 *--------------------------------------------------------*/

/**
 * Fills the empty cells of the grid with new content
 * falling from above.
 */
bool FallState::fillGrid()
{
    bool hasFallen{false};
    for (auto &a: grid) {
        bool curr = makeFall(a.getIndex());
        hasFallen = hasFallen || curr;
    }
    return hasFallen;
}

//ok, get comment
bool FallState::makeFall(const Point &p)
{
    bool hasFallen = false;

    if (!grid.at(p).isEmpty() && grid.at(p).isContentMovable()) {
        for (auto &d: {Direction::South, Direction::SouthWest, Direction::SouthEast}) {
            if (canFallTo(p, d)) {
                if (grid.at(p).moveContentTo(grid.at(p, d))) {
                    hasFallen = true;
                }
                break;
            }
        }
    }

    // TODO rework this part
    if (grid.at(p).isEmpty() && p.y == static_cast<int>(grid.rowCount()-1)) {
        Cell buffer{grid.at(p).getCenter() - Point{0, grid.getRowSize()}, 0, 0, {-1, -1}, grid};
        buffer.setContent(std::make_shared<StandardCandy>(grid, &buffer, buffer.getCenter(), grid.getCellContentSide(), static_cast<StandardCandy::Color>(std::rand()%grid.getCandyColorRange())));
        buffer.moveContentTo(grid.at(p));
        hasFallen = true;
    }
    return hasFallen;
}

/**
 * Whether a given content can fall to the given direction
 * from his current position.
 */
bool FallState::canFallTo(const Point &p, Direction target)
{
    bool canFall{false};

    if (target == Direction::South) {
        canFall = grid.isIndexValid(p, Direction::South)
            && grid.at(p, Direction::South).isEmpty();

    } else if (target==Direction::SouthWest || target==Direction::SouthEast) {
        Direction helper{ target==Direction::SouthWest ? Direction::West : Direction::East};
        canFall =
            grid.isIndexValid(p, target)
            && grid.at(p, target).isEmpty()
            && (
                    (grid.isIndexValid(p, helper)
                    && !grid.at(p, helper).isEmpty()
                    && !grid.at(p, helper).isContentMovable())
                    || (
                        !isFillableByFall(grid.at(p, target).getIndex())
                        && !isFillableByFall(p)
                       )
               );
    }

    return canFall;
}

/**
 * Return whether a cell can be filled by vertical fall
 *
 * Conditions:
 *  - a cell above is not movable;
 *  - cells between the current and the one note movable are empty
 *      and not buffers
 */
bool FallState::isFillableByFall(const Point &point)
{
    bool fillable{true};
    bool empty{true};
    Point current{point};

    while (empty && grid.isIndexValid(current, Direction::North)) {
        current = grid.at(current, Direction::North).getIndex();
        empty = grid.at(current).isEmpty();
        if (!empty) {
            fillable = grid.at(current).isContentMovable();
        }
    }

    return fillable;
}

void FallState::gridAnimationFinished(const Point &p)
{
    waitingList.push_back(p);

    if (!isWaiting()) {
        assert(!grid.animationPlaying());

        bool moreFall = fillGrid();
        if (!moreFall) {

            for (auto &i: grid) {
                processCombinationContaining(i.getIndex());
            }

            notifyCells(Event::FallStateEnd);

            if (isWaiting())
                level.setState(std::make_shared<ClearState>(level, grid));
            else {
                level.setState(std::make_shared<ReadyState>(level, grid));
                level.update(Event::TurnEnd);
            }
        }
    }
}

/*----------------------------------------------------------
 * SwapState
 *--------------------------------------------------------*/

void SwapState::gridAnimationFinished(const Point &p)
{
    waitingList.push_back(p);

    if (!isWaiting()) {
        assert(!grid.animationPlaying());

        if (!swapBack) {
            grid.at(waitingList.at(0)).contentWasSwappedWith(waitingList.at(1));
            if (!isWaiting()) { grid.at(waitingList.at(1)).contentWasSwappedWith(waitingList.at(0)); }

            if (!isWaiting()) {
                for (auto &i: waitingList) {
                    processCombinationContaining(i);
                }
            }
        }

        // TODO maybe put in ready state,
        // on waitingList or the whole grid ?
        for (auto &p: waitingList)
            grid.at(p).setLastSelected(false);

        if (isWaiting()) {
            level.setState(std::make_shared<ClearState>(level, grid));
        } else if (!swapBack) {
            grid.swapCellContent(waitingList);
            swapBack = true;
        } else {
            level.setState(std::make_shared<ReadyState>(level, grid));
        }
    }
}

/*----------------------------------------------------------
 * ClearState
 *--------------------------------------------------------*/

void ClearState::gridAnimationFinished(const Point &p)
{
    waitingList.push_back(p);

    if (!isWaiting()) {
        assert(!grid.animationPlaying());

        // TODO clear from the cell
        for (auto &i: waitingList)
            grid.at(i).removeContent();

        level.setState(std::make_shared<FallState>(level, grid));
    }
}

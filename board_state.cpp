#include "board_state.hpp"

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
        if (!c.isEmpty()) {
            c.getContent()->update(e);
        }
    }
}

/*----------------------------------------------------------
 * MessageShower
 *--------------------------------------------------------*/

MessageShower::MessageShower(Grid &grid, std::string msg) noexcept
    :
        State{grid},
        DrawableContainer{std::make_shared<Rectangle>(grid.getCenter(), 400, 50, FL_WHITE)},
        message{grid.getCenter(), msg}
{
    addAnimation(std::make_shared<StillAnimation>(120));
}

void MessageShower::draw()
{
    DrawableContainer::draw();
    message.draw();
    if (messageFinished)
        grid.setState(std::make_shared<ReadyState>(grid, true));
}

void MessageShower::animationFinished(AnimationT animationType)
{
    switch (animationType) {
        case AnimationT::StillAnimation:
            messageFinished = true;
            break;
        case AnimationT::ScaleAnimation:
        case AnimationT::MoveAnimation:
            break;
    }
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
 * ReadyState
 *--------------------------------------------------------*/

ReadyState::ReadyState(Grid &grid, bool initG) noexcept
    : MatchState{grid}
{
    std::cout << "Entering Ready state" << std::endl;
    if (initG) {
        initGrid();
    }
    hasPossibleAction = isActionPossible();
    std::cout << (hasPossibleAction ? "More action" : "No more action") << std::endl;
}

void ReadyState::initGrid()
{
    replaceGrid();

    // ColourBomb with Standard
    Point point{1, 1};
    grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Red);
    point = {1, 2};
    grid.put(point, ContentT::StandardCandy, StandardCandy::Color::Blue);

    point = {4, 4};
    grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);
    point = {0, 5};
    grid.put(point, ContentT::StripedCandy, StandardCandy::Color::Blue, Axis::Horizontal);

    // ColourBomb with Wrapped
    /*Point point{1, 1};
    grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Red);
    point = {1, 2};
    grid.put(point, ContentT::WrappedCandy, StandardCandy::Color::Blue);*/

    // ColourBomb with Striped
    /*Point point{1, 1};
    grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Red);
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

    // add 3 walls
    point = {2, 5};
    grid.put(point, ContentT::Wall, StandardCandy::Color::Blue);
    point = {3, 5};
    grid.put(point, ContentT::Wall, StandardCandy::Color::Blue);
    point = {4, 5};
    grid.put(point, ContentT::Wall, StandardCandy::Color::Blue);

    // add an icing
    /*point = {5, 4};
    grid.put(point, ContentT::Icing, StandardCandy::Color::Blue);*/
}

void ReadyState::draw()
{
    if (!hasPossibleAction)
        grid.setState(std::make_shared<MessageShower>(grid, "No more combinations"));
}

// TODO
void ReadyState::replaceGrid()
{
    for (auto &c: grid) {
        c.setContent(std::make_shared<StandardCandy>(grid, &c, c.getCenter(), grid.getCellContentSide()));
        while (isInCombination(c.getIndex())) {
            c.removeContent();
            c.setContent(std::make_shared<StandardCandy>(grid, &c, c.getCenter(), grid.getCellContentSide()));
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

void ReadyState::gridAnimationFinished(const Point &p)
{
    throw std::runtime_error("There should be no animations in ReadyState");
}

void ReadyState::mouseMove(Point mouseLoc)
{
    for (auto &c: grid)
        c.mouseMove(mouseLoc);
}

void ReadyState::mouseClick(Point mouseLoc)
{
    for (auto &c: grid)
        c.mouseClick(mouseLoc);
}

void ReadyState::mouseDrag(Point mouseLoc)
{
    for (auto &c: grid)
        c.mouseDrag(mouseLoc);
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
            grid.setState(std::make_shared<SwapState>(grid));
        }
    }
}

/*----------------------------------------------------------
 * MatchState
 *--------------------------------------------------------*/

bool MatchState::processCombinationsFrom(const Point &origin)
{
    if (grid.at(origin).isContentClearing())
        return false;

    auto combi = combinationsFrom(origin);
    Point point = combi.at(2).at(0);
    bool oneCombination{false};

    const int V = 0;
    const int H = 1;

    std::size_t vc = combi.at(0).size();  // vertical count
    std::size_t hc = combi.at(1).size();  // horizontal count

    // 3 in one axis
    if ((vc==2 && hc<2) || (hc==2 && vc<2)) {
        auto toClear{ combi.at(hc>vc ? H : V) };
        grid.clearCell(point);
        grid.clearCell(toClear);
        oneCombination = true;

    // 4 in one axis
    } else if ((vc==3 && hc<2) || (hc==3 && vc<2)) {
        auto toClear{ combi.at(hc>vc ? H : V) };
        StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(grid.at(point).getContent())->getColor()};  // TODO simplify this
        /* grid.clearCellWithoutAnimation(point);  //TODO function in grid */
        grid.at(point).clearWithoutAnimation();
        grid.clearCell(toClear);
        grid.put(point, ContentT::StripedCandy, color, vc<hc ? Axis::Vertical : Axis::Horizontal);
        /* nextStateCount += toClear.size(); */
        oneCombination = true;

    // 5 or more in one axis
    } else if ((vc>=4 && hc<2) || (hc>=4 && vc<2)) {
        auto toClear{ combi.at(hc>vc ? H : V) };
        /* grid.clearCellWithoutAnimation(point);  //TODO function in grid */
        grid.at(point).clearWithoutAnimation();
        grid.clearCell(toClear);
        grid.put(point, ContentT::ColourBomb);
        /* nextStateCount += toClear.size(); */
        oneCombination = true;

    // More than 3 on both axis
    } else if (vc>=2 && hc>=2) {
        StandardCandy::Color color {std::dynamic_pointer_cast<StandardCandy>(grid.at(point).getContent())->getColor()};
        grid.at(point).clearWithoutAnimation();
        for (auto &a: combi) {
            grid.clearCell(a);
        }
        grid.put(point, ContentT::WrappedCandy, color);
        oneCombination = true;
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
std::vector<std::vector<Point>> MatchState::combinationsFrom(const Point &origin, bool rec)
{
    std::vector<std::vector<Point>> ret {{}, {}, {origin}};

    for (size_t axis = 0; axis<2; ++axis) {      // Axis : Vertical | Horizontal
        for (size_t card = 0; card<2; ++card) {  // Cardinality : North/South | West/East

            Direction direction{ static_cast<Direction>(2*axis + card) };
            Point curr{origin};

            while (
                    grid.isIndexValid(curr, direction)
                    && !grid.at(curr, direction).isEmpty()
                    && grid.at(curr).hasMatchWith(grid.at(curr, direction).getIndex())
                  )
            {
                curr = grid.at(curr, direction).getIndex();
                ret[axis].push_back(curr);
            }
        }
    }

    if (rec) {
        int retSize = ret.at(0).size() + ret.at(1).size();

        for (size_t axis = 0; axis<2; ++axis) {
            for (auto &point : ret[axis]) { 
                auto temp { combinationsFrom(point, false) };
                unsigned tempSize = temp.at(0).size() + temp.at(1).size();
                if (tempSize>retSize) {
                    ret = temp;
                    retSize = tempSize;
                }
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
    auto combi = combinationsFrom(point);
    return combi.at(0).size() >= 2
        || combi.at(1).size() >= 2;
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
    if (grid.at(p).isEmpty() && p.y == grid.rowCount()-1) {
        Cell buffer{grid.at(p).getCenter() - Point{0, grid.getRowSize()}, 0, 0, {-1, -1}, grid};
        buffer.setContent(std::make_shared<StandardCandy>(grid, &buffer, buffer.getCenter(), grid.getCellContentSide()));
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
                    grid.isIndexValid(p, helper)
                    && !grid.at(p, helper).isEmpty()
                    && !grid.at(p, helper).isContentMovable()
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
                processCombinationsFrom(i.getIndex());
            }

            notifyCells(Event::FallStateEnd);

            if (isWaiting())
                grid.setState(std::make_shared<ClearState>(grid));
            else
                grid.setState(std::make_shared<ReadyState>(grid));
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
            grid.at(waitingList.at(1)).contentWasSwappedWith(waitingList.at(0));
            if (!isWaiting()) {
                for (auto &i: waitingList) {
                    processCombinationsFrom(i);
                }
            }
        }

        if (isWaiting()) {
            grid.setState(std::make_shared<ClearState>(grid));
        } else if (!swapBack) {
            grid.swapCellContent(waitingList);
            swapBack = true;
        } else {
            grid.setState(std::make_shared<ReadyState>(grid));
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

        grid.setState(std::make_shared<FallState>(grid));
    }
}

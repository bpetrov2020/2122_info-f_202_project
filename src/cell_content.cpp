#include "cell_content.hpp"
#include "grid.hpp"

/*----------------------------------------------------------
 * CellContent
 *--------------------------------------------------------*/

CellContent::CellContent(
        Grid &grid,
        Cell *cell,
        std::shared_ptr<Shape> drawable
        )
    :
        DrawableContainer{drawable},
        grid{grid},
        containerCell{cell}
{ }

void CellContent::animationFinished(AnimationT animationType)
{
    if (animationType == AnimationT::PulseAnimation) {
        /* containerCell->update(Event::PulseAnimationFinished); */
        m_isPulsing = false;
        m_pulseFinished = true;
        /* grid.cellContentAnimationFinished(containerCell->getIndex()); */
    }
}

void CellContent::draw()
{
    DrawableContainer::draw();
    if (m_pulseFinished) {
        m_pulseFinished = false;
        grid.cellContentAnimationFinished(containerCell->getIndex());
    }

}

/*----------------------------------------------------------
 * ClearableCellContent
 *--------------------------------------------------------*/

ClearableCellContent::ClearableCellContent(
        Grid &grid,
        Cell *cell,
        std::shared_ptr<Shape> drawable,
        bool clearableByOther
        )
    :
        CellContent{
            grid,
            cell,
            drawable
        },
        clearableByOther{clearableByOther}
{ }

void ClearableCellContent::draw()
{
    if (clearFinished) {
        clearFinished = false;
        grid.cellContentAnimationFinished(containerCell->getIndex());
        /* containerCell->update(Event::cellContentAnimationFinished); */
    }
}

void ClearableCellContent::clearWithoutEffect()
{
    m_isClearing = true;
    addAnimation(std::make_shared<ScaleAnimation>(20));
}

void ClearableCellContent::clear()
{
    clearWithoutAnimation();
    addAnimation(std::make_shared<ScaleAnimation>(20));
}

void ClearableCellContent::clearWithoutAnimation()
{
    m_isClearing = true;
}

void ClearableCellContent::animationFinished(AnimationT animationType)
{
    if (animationType == AnimationT::ScaleAnimation) {
        clearFinished = true;
        m_isClearing = false;
    }
}

/*----------------------------------------------------------
 * MovableCellContent
 *--------------------------------------------------------*/

MovableCellContent::MovableCellContent(
        Grid &grid,
        Cell *cell,
        std::shared_ptr<Shape> drawable
        )
    :
        CellContent{
            grid,
            cell,
            drawable
        }
{ }

void MovableCellContent::draw()
{
    if (moveFinished) {
        moveFinished = false;
        /* grid.moveDone(); */
        grid.cellContentAnimationFinished(containerCell->getIndex());
    }
}

void MovableCellContent::moveTo(const Point &point)
{
    moveToWithoutAnimation(point);
    addAnimation(std::make_shared<MoveAnimation>(ANIM_TIME, getCenter(), grid.at(point).getCenter()));
}

void MovableCellContent::moveToWithoutAnimation(const Point &point)
{
    m_isMoving = true;
    containerCell = &grid.at(point);
}

void MovableCellContent::wasSwappedWith(const Point &)
{

}

void MovableCellContent::animationFinished(AnimationT a)
{
    if (a == AnimationT::MoveAnimation) {
        /* setCenter(std::dynamic_pointer_cast<MoveAnimation>(animation)->getEnd()); */
        moveFinished = true;
        m_isMoving = false;
    }
}

/*----------------------------------------------------------
 * MatchableCellContent
 *--------------------------------------------------------*/

MatchableCellContent::MatchableCellContent(
        Grid &grid,
        Cell *cell,
        std::shared_ptr<Shape> drawable
        )
    :
        CellContent{
            grid,
            cell,
            drawable
        }
{ }

/*----------------------------------------------------------
 * TextContent
 *--------------------------------------------------------*/

TextContent::TextContent(Grid &grid, Cell *cell, const Point &center, int side, std::string text, int fontSize, Fl_Color textColor)
    : CellContent {
        grid,
        cell,
        std::make_shared<Text>(center, text, fontSize, textColor)
    }
{ }

/*----------------------------------------------------------
 * Wall
 *--------------------------------------------------------*/

Wall::Wall(
        Grid &grid,
        Cell *cell,
        const Point &center,
        int side
        )
    :
        CellContent{
            grid,
            cell,
            std::make_shared<Rectangle>(center, side, side, FL_BLACK)
        }
{ }

/*----------------------------------------------------------
 * Icing
CellContent
 *--------------------------------------------------------*/

Icing::Icing(
        Grid &grid,
        Cell *cell,
        const Point &center,
        int side,
        int layers
        )
    :
        CellContent{
            grid,
            cell,
            std::make_shared<Rectangle>(center, side, side, FL_CYAN)
        },
        ClearableCellContent{
            grid,
            cell,
            std::make_shared<Rectangle>(center, side, side, FL_CYAN),
            true
        },
        layers{layers},
        num{center, std::to_string(layers)}
{ }

void Icing::removeLayer()
{
    assert(layers>0);
    --layers;
    num.setString(std::to_string(layers));
    grid.update(Event::IcingCleared);
}

void Icing::draw()
{
    CellContent::draw();
    ClearableCellContent::draw();
    if (getLayers() != 0)
        num.draw();
}

void Icing::clear()
{
    removeLayer();
    if (getLayers() == 0)
        ClearableCellContent::clear();
}

void Icing::update(Event e)
{
    switch (e) {
        case Event::NeighbourMatched:
            if (layers>0 && !isClearing())
                clear();
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------
 * StandardCandy
 *--------------------------------------------------------*/

StandardCandy::StandardCandy(
        Grid &grid,
        Cell *cell,
        Color color,
        std::shared_ptr<AnimatableShape> shape
        )
    :
        CellContent{
            grid,
            cell,
            shape
        },
        ClearableCellContent{grid, cell, shape, true},
        MovableCellContent{grid, cell, shape},
        MatchableCellContent{grid, cell, shape},
        color{color}
{ }

StandardCandy::StandardCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side
        )
    :
        StandardCandy{
            grid,
            cell,
            center,
            side,
            static_cast<StandardCandy::Color>(std::rand()%6)  // if no color provided, pick random
        }
{ }

StandardCandy::StandardCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side,
        Color color)
    :
        StandardCandy{
            grid,
            cell,
            color,
            std::make_shared<Rectangle>(center, side, side, flRelative[static_cast<int>(color)])
        }
{ }

void StandardCandy::draw()
{
    CellContent::draw();
    MovableCellContent::draw();
    ClearableCellContent::draw();
}

void StandardCandy::animationFinished(AnimationT a)
{
    CellContent::animationFinished(a);
    MovableCellContent::animationFinished(a);
    ClearableCellContent::animationFinished(a);
}

void StandardCandy::clearWithoutAnimation()
{
    ClearableCellContent::clearWithoutAnimation();
    for (auto &n: grid.neighboursOf(containerCell->getIndex())) {
        auto content {grid.at(n).getContent()};
        if (content)
            content->update(Event::NeighbourCleared);
    }
}

/* bool StandardCandy::operator==(CellContent &other) const */
/* { */
/*     try { */
/*         StandardCandy& otherCandy { dynamic_cast<StandardCandy&>(other) }; */
/*         return getColor() == otherCandy.getColor(); */
/*     } catch (const std::bad_cast& err) { */
/*         throw std::runtime_error("StandardCandy::operator== -> provided argument could not be converted to a StandardCandy"); */
/*     } */
/* } */

bool StandardCandy::hasMatchWith(const Point &point) const
{
    std::shared_ptr<StandardCandy> other {std::dynamic_pointer_cast<StandardCandy>(grid.at(point).getContent())};
    return other && getColor() == other->getColor();
}

/*----------------------------------------------------------
 * StripedCandy
 *--------------------------------------------------------*/

StripedCandy::StripedCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side,
        StandardCandy::Color color,
        Axis axis
        )
    :
        CellContent{
            grid,
            cell,
            std::make_shared<StripedRectangle>(center, side, side, axis, flRelative[static_cast<int>(color)])
        },
        StandardCandy{
            grid,
            cell,
            color,
            std::make_shared<StripedRectangle>(center, side, side, axis, flRelative[static_cast<int>(color)])
        },
        axis{axis}
{ }

StripedCandy::StripedCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side,
        StandardCandy::Color color)
    :
        StripedCandy(
            grid,
            cell,
            center,
            side,
            color,
            static_cast<Axis>(std::rand()%2)  // if no axis provided, pick random
        )
{ }

void StripedCandy::clearWithoutAnimation()
{
    if (doubleStriped) {                    // case two striped are switched
        doubleStripedClear();
    } else if (wrappedWithStriped) {        // case wrapped is switched with striped
        wrappedWithStripedClear();
    } else {                                // normal clear case
        regularClear();
    }
}

void StripedCandy::doubleStripedClear()
{
    StandardCandy::clearWithoutAnimation();

    for (unsigned i=0; i<grid.colCount(); ++i) {
        grid.clearCell(Point{static_cast<int>(i), containerCell->getIndex().y});
    }
    for (unsigned i=0; i<grid.rowCount(); ++i) {
        grid.clearCell(Point{containerCell->getIndex().x, static_cast<int>(i)});
    }
}

void StripedCandy::wrappedWithStripedClear()
{
    StandardCandy::clearWithoutAnimation();

    for (auto &p: {Point{-1, 0}, Point{0, 0}, Point{1, 0}}) {
        Point decal {containerCell->getIndex() + p};
        if (grid.isIndexValid(decal))
            for (unsigned y=0; y<grid.rowCount(); ++y)
                grid.clearCell({decal.x, static_cast<int>(y)});
    }

    for (auto &p: {Point{0, -1}, Point{0, 0}, Point{0, 1}}) {
        Point decal {containerCell->getIndex() + p};
        if (grid.isIndexValid(decal))
            for (unsigned x=0; x<grid.rowCount(); ++x)
                grid.clearCell({static_cast<int>(x), decal.y});
    }
}

void StripedCandy::regularClear()
{
    StandardCandy::clearWithoutAnimation();
    if (axis == Axis::Horizontal) {
        for (unsigned i=0; i<grid.colCount(); ++i) {
            grid.clearCell(Point{static_cast<int>(i), containerCell->getIndex().y});
        }
    } else if (axis == Axis::Vertical) {
        for (unsigned i=0; i<grid.rowCount(); ++i) {
            grid.clearCell(Point{containerCell->getIndex().x, static_cast<int>(i)});
        }
    }
}

void StripedCandy::wasSwappedWith(const Point &p)
{
    if (!containerCell->isLastSelected()) { return; }

    std::shared_ptr<StripedCandy> striped {std::dynamic_pointer_cast<StripedCandy>(grid.at(p).getContent())};
    std::shared_ptr<WrappedCandy> wrapped {std::dynamic_pointer_cast<WrappedCandy>(grid.at(p).getContent())};

    if (striped) {      // case other is a stripped
        doubleStriped = true;
        striped->clearWithoutEffect();  // clears the other striped like it is a standard candy
        clear();
    } else if (wrapped) {   // case wrapped candy
        wrappedWithStriped = true;
        wrapped->clearWithoutEffect();  // clears the other wrapped like it is a standard candy
        clear();
    }
}

/*----------------------------------------------------------
 * WrappedCandy
 *--------------------------------------------------------*/

WrappedCandy::WrappedCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side,
        StandardCandy::Color color
        )
    :
        CellContent{
            grid,
            cell,
            std::make_shared<Star>(center, side, side, flRelative[static_cast<int>(color)])
        },
        StandardCandy{
                grid,
                cell,
                color,
                std::make_shared<Star>(center, side, side, flRelative[static_cast<int>(color)])
        }
{ }

void WrappedCandy::clear()
{
    if (secondPhase) {
        StandardCandy::clear();
    } else {
        clearWithoutAnimation();
        secondPhase = true;
        std::shared_ptr<Star> drawableStar { std::dynamic_pointer_cast<Star>(drawable)};
        drawableStar->setSecondPhase();
    }
}

void WrappedCandy::update(Event e)
{
    switch (e) {
        case Event::FallStateEnd:
            if (secondPhase)
                StandardCandy::clear();
            break;
        default:
            break;
    }
}

void WrappedCandy::clearWithoutAnimation()
{
    if (doubleWrapped) {                    // case two wrapped are switched
        doubleWrappedClear();
    } else if (wrappedWithStriped) {        // case wrapped is switched with striped
        wrappedWithStripedClear();
    } else {                                // normal clear case
        regularClear();
    }
}

void WrappedCandy::doubleWrappedClear()
{
    StandardCandy::clearWithoutAnimation();

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            Point p{x, y};
            try {
                grid.clearCell(p+containerCell->getIndex());
            } catch (const std::out_of_range& err) {}
        }
    }
}

void WrappedCandy::wrappedWithStripedClear()
{
    clearWithoutEffect();

    for (auto &p: {Point{-1, 0}, Point{0, 0}, Point{1, 0}}) {
        Point decal {containerCell->getIndex() + p};
        if (grid.isIndexValid(decal))
            for (unsigned y=0; y<grid.rowCount(); ++y)
                grid.clearCell({decal.x, static_cast<int>(y)});
    }

    for (auto &p: {Point{0, -1}, Point{0, 0}, Point{0, 1}}) {
        Point decal {containerCell->getIndex() + p};
        if (grid.isIndexValid(decal))
            for (unsigned x=0; x<grid.rowCount(); ++x)
                grid.clearCell({static_cast<int>(x), decal.y});
    }
}

void WrappedCandy::regularClear()
{
    StandardCandy::clearWithoutAnimation();

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

    for (auto &p: directionModifier) {
        try {
            grid.clearCell(p+containerCell->getIndex());
        } catch (const std::out_of_range& err) {}
    }
}

void WrappedCandy::wasSwappedWith(const Point &p)
{
    if (!containerCell->isLastSelected()) { return; }

    std::shared_ptr<StripedCandy> striped {std::dynamic_pointer_cast<StripedCandy>(grid.at(p).getContent())};
    std::shared_ptr<WrappedCandy> wrapped {std::dynamic_pointer_cast<WrappedCandy>(grid.at(p).getContent())};

    if (striped) {      // case other is a stripped
        wrappedWithStriped = true;
        striped->clearWithoutEffect();  // clears the other striped like it is a standard candy
        clear();
    } else if (wrapped) {   // case wrapped candy
        doubleWrapped = true;
        wrapped->clearWithoutEffect();  // clears the other wrapped like it is a standard candy
        clear();
    }
}

/*----------------------------------------------------------
 * ColourBomb
 *--------------------------------------------------------*/

ColourBomb::ColourBomb(
        Grid &grid,
        Cell *cell,
        Point center,
        int side
)
        :
        CellContent{
                grid,
                cell,
                std::make_shared<MulticolourCircle>(center, side)
        },
        ClearableCellContent{grid, cell, std::make_shared<MulticolourCircle>(center, side), true},
        MovableCellContent{grid, cell, std::make_shared<MulticolourCircle>(center, side)}
{ }

void ColourBomb::draw()
{
    CellContent::draw();
    MovableCellContent::draw();
    ClearableCellContent::draw();
}

void ColourBomb::animationFinished(AnimationT a)
{
    CellContent::animationFinished(a);
    MovableCellContent::animationFinished(a);
    ClearableCellContent::animationFinished(a);
}

void ColourBomb::replaceAndExplode()
{
    // case other is a ColourBomb
    if (typeToReplaceWith == ContentT::ColourBomb) {
        for (auto &c: grid) {
            grid.clearCell(c.getIndex());
        }
        return;
    }

    // case other is standard, striped or wrapped
    for (auto &c: grid) {
        if (!c.isEmpty() && c.getContent()->getType() == ContentT::StandardCandy) {
            StandardCandy::Color cellColor{
                    std::dynamic_pointer_cast<StandardCandy>(c.getContent())->getColor()};
            if (cellColor == colorToReplace) {
                // place the specific candy
                c.clearWithoutAnimation();
                grid.put(c.getIndex(), typeToReplaceWith, cellColor);
            }
        }
    }

    for (auto &c: grid) {
        // the next cast test is to avoid calling getColor() on contents that don't have a color
        if (!c.isEmpty() && std::dynamic_pointer_cast<StandardCandy>(c.getContent())) {
            StandardCandy::Color cellColor{
                    std::dynamic_pointer_cast<StandardCandy>(c.getContent())->getColor()};
            if (cellColor == colorToReplace) {
                // explode all specific candies
                grid.clearCell(c.getIndex());
            }
        }
    }
}

void ColourBomb::clearWithoutAnimation()
{
    ClearableCellContent::clearWithoutAnimation();
    if (!wasSwapped)
        colorToReplace = getColorToClear();

    replaceAndExplode();
}

void ColourBomb::wasSwappedWith(const Point &p)
{
    std::shared_ptr<StandardCandy> otherCandy {std::dynamic_pointer_cast<StandardCandy>(grid.at(p).getContent())};
    std::shared_ptr<ColourBomb> otherBomb {std::dynamic_pointer_cast<ColourBomb>(grid.at(p).getContent())};

    // case StandardCandy, StripedCandy, WrappedCandy
    if (otherCandy) {
        colorToReplace = otherCandy->getColor();
        typeToReplaceWith = otherCandy->getType();
        wasSwapped = true;
        clear();

    // case ColourBomb
    } else if (otherBomb && !hasAnimation()) {    // checking animation to prevent both bombs to explode
        typeToReplaceWith = otherBomb->getType();
        wasSwapped = true;
        clear();
    }


}

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

void MovableCellContent::wasSwappedWith(const Point &p)
{
    std::cout << "I was swapped with " << p << std::endl;
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
        MatchableCellContent{grid, cell, shape},
        MovableCellContent{grid, cell, shape},
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
    DrawableContainer::draw();
    MovableCellContent::draw();
    ClearableCellContent::draw();
}

void StandardCandy::animationFinished(AnimationT a)
{
    MovableCellContent::animationFinished(a);
    ClearableCellContent::animationFinished(a);
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

void StripedCandy::clearWithoutAnimation()
{
    StandardCandy::clearWithoutAnimation();
    if (axis == Axis::Horizontal) {
        for (unsigned i=0; i<grid.colCount(); ++i) {
            grid.clearCell(Point{i, containerCell->getIndex().y});
        }
    } else if (axis == Axis::Vertical) {
        for (unsigned i=0; i<grid.rowCount(); ++i) {
            grid.clearCell(Point{containerCell->getIndex().x, i});
        }
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
        std::cout <<"oneclear";
        secondPhase = true;
    }
}

void WrappedCandy::update(Event e)
{
    switch (e) {
        case Event::FallStateEnd:
            if (secondPhase)
                StandardCandy::clear();
            break;
    }
}

void WrappedCandy::clearWithoutAnimation()
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

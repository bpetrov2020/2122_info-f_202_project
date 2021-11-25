#include "cell_content.hpp"
#include "grid.hpp"

/*----------------------------------------------------------
 * CellContent
 *--------------------------------------------------------*/

CellContent::CellContent(
        Grid &grid,
        Cell *cell,
        std::shared_ptr<Shape> drawable,
        bool movable,
        bool matchable,
        bool clearable)
    :
        DrawableContainer{drawable},
        grid{grid},
        containerCell{cell},
        movable{movable},
        matchable{matchable},
        clearable{clearable}
{ }

void CellContent::animationFinished(AnimationT animationType)
{
    DrawableContainer::animationFinished(animationType);
    switch (animationType) {
        case AnimationT::MoveAnimation:
            setCenter(std::dynamic_pointer_cast<MoveAnimation>(animation)->getEnd());
            moveFinished = true;
            break;
        case AnimationT::ScaleAnimation:
            clearFinished = true;
            break;
        case AnimationT::StillAnimation:
            break;
    }
}

void CellContent::moveTo(const Point& cell)
{
    addAnimation(std::make_shared<MoveAnimation>(ANIM_TIME, getCenter(), grid.at(cell).getCenter()));
    containerCell = &grid.at(cell);
}

bool CellContent::isClearing()
{
    return m_isClearing;
}

bool CellContent::isMoving()
{
    return animation && animation->type() == AnimationT::MoveAnimation;
}

void CellContent::clear()
{
    clearWithoutAnimation();
    addAnimation(std::make_shared<ScaleAnimation>(20));
}

void CellContent::clearWithoutAnimation()
{
    m_isClearing = true;
}

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
            std::make_shared<Rectangle>(center, side, side, FL_BLACK),
            false,
            false,
            false
        }
{ }

bool Wall::operator==(CellContent &other) const
{
    return true;
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
            shape,
            true,
            true
        },
        color{color}
{}

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

void StandardCandy::draw()
{
    DrawableContainer::draw();

    // Send messages to the grid
    if (moveFinished){
        grid.moveDone();
        moveFinished = false;
    }
    if (clearFinished) {
        grid.clearDone();
        clearFinished = false;
    }
}

bool StandardCandy::operator==(CellContent &other) const
{
    try {
        StandardCandy& otherCandy { dynamic_cast<StandardCandy&>(other) };
        return getColor() == otherCandy.getColor();
    } catch (const std::bad_cast& err) {
        throw std::runtime_error("StandardCandy::operator== -> provided argument could not be converted to a StandardCandy");
    }
}


/* bool StandardCandy::hasMatch(const StandardCandy &other) const */
/* { */
/*     return getColor() == other.getColor(); */
/* } */

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
        StandardCandy{
                grid,
                cell,
                color,
                std::make_shared<Star>(center, side, side, flRelative[static_cast<int>(color)])
        }
{ }

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

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
    return animation && animation->type() == AnimationT::ScaleAnimation;
}

bool CellContent::isMoving()
{
    return animation && animation->type() == AnimationT::MoveAnimation;
}

/*----------------------------------------------------------
 * StandardCandy
 *--------------------------------------------------------*/

StandardCandy::StandardCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side)
    :
        StandardCandy{
                grid,
                cell,
                center,
                side,
                static_cast<StandardCandy::Color>(std::rand()%6)}  // if no color provided, pick random
{ }

StandardCandy::StandardCandy(
        Grid &grid,
        Cell *cell,
        Point center,
        int side,
        StandardCandy::Color color)
    :
        CellContent{
            grid,
            cell,
            std::make_shared<Rectangle>(center, side, side, flRelative[static_cast<int>(color)]),
            true,
            true},
        color{color}
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

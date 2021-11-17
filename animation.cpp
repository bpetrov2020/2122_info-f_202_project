#include "animation.hpp"

/*----------------------------------------------------------
 * StillAnimation
 *--------------------------------------------------------*/

void StillAnimation::draw()
{
    if (drawable) {
        ++elapsed;
        drawable->draw();
    }
}

/*----------------------------------------------------------
 * ScaleAnimation
 *--------------------------------------------------------*/

double ScaleAnimation::currentScale() const
{
    if (!isComplete())
        return 1-(0.95/duration * elapsed);
    else
        return 0;
}

void ScaleAnimation::draw()
{
    if (drawable) {
        ++elapsed;
        Scale s{drawable->getCenter(), currentScale()};
        drawable->draw();
    }
}

/*----------------------------------------------------------
 * MoveAnimation
 *--------------------------------------------------------*/

Point MoveAnimation::currentTranslation() const
{
    if (!isComplete())
        return (end-start)/duration * elapsed;
    else
        return {0, 0};
}

void MoveAnimation::draw()
{
    if (drawable) {
        ++elapsed;
        Translation t{ currentTranslation() };
        drawable->draw();
    }
}

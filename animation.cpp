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
    if (!isComplete()) {
        double x = (end.x-start.x)/static_cast<double>(duration) * elapsed;
        double y = (end.y-start.y)/static_cast<double>(duration) * elapsed;
        return {static_cast<int>(x), static_cast<int>(y)};
    }
    else
        return end-start;
}

void MoveAnimation::draw()
{
    if (drawable) {
        ++elapsed;
        drawable->setCenter(getStart()+currentTranslation());
        /* Translation t{ currentTranslation() }; */
        drawable->draw();
    }
}

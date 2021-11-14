#include "animation.hpp"

/*----------------------------------------------------------
                        Animation
 *--------------------------------------------------------*/

template <typename S>
Animation<S>::Animation(S* drawable, int duration)
    : drawable{drawable}, duration{duration} { }

template <typename S>
bool Animation<S>::isComplete() const
{
    return elapsed>duration;
}

/*----------------------------------------------------------
                        StillAnimation
 *--------------------------------------------------------*/

template <typename S>
StillAnimation<S>::StillAnimation(S* drawable, int duration)
    : Animation<S>{drawable, duration} { }

template <typename S>
void StillAnimation<S>::draw()
{
    ++this->elapsed;
    this->drawable->drawWithoutAnimate();
}

/*----------------------------------------------------------
                        ClearAnimation
 *--------------------------------------------------------*/

template <typename S>
ClearAnimation<S>::ClearAnimation(S* drawable, int duration)
    : Animation<S>{drawable, duration} { }

template <typename S>
double ClearAnimation<S>::currentScale() const
{
    if (!this->isComplete())
        return 1-(0.95/this->duration * time);
    else
        return 0;
}

template <typename S>
void ClearAnimation<S>::draw()
{
    ++this->elapsed;
    Scale s{this->drawable->getCenter(), currentScale()};
    this->drawable->drawWithoutAnimate();
}

/*----------------------------------------------------------
                        MoveAnimation
 *--------------------------------------------------------*/

template <typename S>
MoveAnimation<S>::MoveAnimation(S* drawable, int duration, Point start, Point end)
    : Animation<S>{drawable, duration}, start{start}, end{end} { }

template <typename S>
Point MoveAnimation<S>::currentTranslation() const
{
    if (!this->isComplete())
        return (end-start)/this->duration * time;
    else
        return {0, 0};
}

template <typename S>
void MoveAnimation<S>::draw()
{
    ++this->elapsed;
    Translation t{ currentTranslation() };
    this->drawable->drawWithoutAnimate();
}


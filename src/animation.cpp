#include "animation.hpp"

/*----------------------------------------------------------
 * StillAnimation
 *--------------------------------------------------------*/

void StillAnimation::draw()
{
    if (isAttachedTo()) {
        ++m_elapsed;
        m_drawable->draw();
    }
}

/*----------------------------------------------------------
 * ClearAnimation
 *--------------------------------------------------------*/

double ClearAnimation::currentScale() const
{
    if (!isComplete())
        return 1-(0.95/m_duration * m_elapsed);
    else
        return 0;
}

void ClearAnimation::draw()
{
    if (isAttachedTo()) {
        ++m_elapsed;
        Scale s{m_drawable->getCenter(), currentScale()};
        m_drawable->draw();
    }
}

/*----------------------------------------------------------
 * MoveAnimation
 *--------------------------------------------------------*/

Point MoveAnimation::currentTranslation() const
{
    if (!isComplete()) {
        double x = (m_end.x-m_start.x)/static_cast<double>(m_duration) * m_elapsed;
        double y = (m_end.y-m_start.y)/static_cast<double>(m_duration) * m_elapsed;
        return {static_cast<int>(x), static_cast<int>(y)};
    }
    else
        return m_end-m_start;
}

void MoveAnimation::draw()
{
    if (isAttachedTo()) {
        ++m_elapsed;
        m_drawable->setCenter(getStart()+currentTranslation());
        /* Translation t{ currentTranslation() }; */
        m_drawable->draw();
    }
}

/*----------------------------------------------------------
 * HintAnimation
 *--------------------------------------------------------*/

double HintAnimation::currentScale()
{
    if (!isComplete())
        return 1 + std::sin(std::numbers::pi*(static_cast<double>(m_elapsed)/m_duration))/4;
    else
        return 0;
}

void HintAnimation::draw()
{
    if (isAttachedTo()) {
        ++m_elapsed;
        Scale s{m_drawable->getCenter(), currentScale()};
        m_drawable->draw();
    }
}

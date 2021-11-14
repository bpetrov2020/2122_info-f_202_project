#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "point.hpp"

#include <FL/fl_draw.H>

/*----------------------------------------------------------
                       Transformations
 *--------------------------------------------------------*/

struct Translation {
    Translation(Point p) {
        fl_push_matrix();
        fl_translate(p.x, p.y);
    }
    ~Translation() {
        fl_pop_matrix();
    }
};

struct Scale {
    Scale(Point center, double factor) {
        fl_push_matrix();
        fl_translate(center.x, center.y);
        fl_scale(factor);
        fl_translate(-1*center.x, -1*center.y);
    }
    ~Scale() {
        fl_pop_matrix();
    }
};

struct Rotation {
    Rotation(Point center, double angle) {
        fl_push_matrix();
        fl_translate(center.x, center.y);
        fl_rotate(angle);
        fl_translate(-1*center.x, -1*center.y);
    }
    ~Rotation() {
        fl_pop_matrix();
    }
};

/*----------------------------------------------------------
                        Animations
 *--------------------------------------------------------*/

template <typename S>
class Animation
{
    protected:
        S* drawable;
        int elapsed = 0;
        int duration;
    public:
        Animation(S* drawable, int duration);
        virtual ~Animation() noexcept = default;

        virtual void draw() = 0;
        bool isComplete() const;
};

/**
 * Animation that doesn't do anything
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 */
template <typename S>
class StillAnimation : public Animation<S>
{
    public:
        StillAnimation(S* drawable, int duration);

        void draw() override;
};

/**
 * Animation played when something is cleared (e.g. a cell)
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 */
template <typename S>
class ClearAnimation : public Animation<S>
{
    private:
        double currentScale() const;
    public:
        ClearAnimation(S* drawable, int duration);

        void draw() override;
};

/**
 * Animation played when an object is moved from a to b
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 * @param start begining point
 * @param end finishing point
 */
template <typename S>
class MoveAnimation : public Animation<S>
{
    private:
        Point start;
        Point end;
        Point currentTranslation() const;
    public:
        MoveAnimation(S* drawable, int duration, Point start, Point end);

        void draw() override;
};

#endif

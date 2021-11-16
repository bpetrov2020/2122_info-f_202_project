#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "point.hpp"

#include <FL/fl_draw.H>
#include <iostream>
#include <memory>

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

class Animation
{
    protected:
        std::shared_ptr<Shape> drawable;
        int elapsed = 0;
        int duration;
    public:
        Animation(int duration, std::shared_ptr<Shape> drawable = nullptr)
            : drawable{drawable}, duration{duration} { }

        Animation(const Animation& a) = delete;
        Animation operator=(const Animation& a) = delete;

        virtual ~Animation() noexcept = default;

        virtual void draw() = 0;
        bool isComplete() const
        {
            return elapsed>duration;
        }
        void attachTo(std::shared_ptr<Shape> drawable_)
        {
            /* std::cout << drawable << std::endl; */
            drawable = drawable_;
            /* std::cout << drawable << std::endl; */
        }
};

/**
 * Animation that doesn't do anything
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 */
class StillAnimation : public Animation
{
    public:
        StillAnimation(int duration, std::shared_ptr<Shape> drawable = nullptr) noexcept
            : Animation{duration, drawable} { }

        void draw() override
        {
            /* std::cout << this->drawable; */
            if (drawable) {
                /* std::cout << "in here"; */
                ++elapsed;
                drawable->draw();
                /* this->drawable->drawWithoutAnimate(); */
            }
        }
};

/**
 * Animation played when something is cleared (e.g. a cell)
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 */
class ClearAnimation : public Animation
{
    private:
        double currentScale() const
        {
            if (!isComplete())
                return 1-(0.95/duration * elapsed);
            else
                return 0;
        }
    public:
        ClearAnimation(int duration, std::shared_ptr<Shape> drawable = nullptr) noexcept
            : Animation{duration, drawable} { }

        void draw() override
        {
            if (drawable) {
                ++elapsed;
                Scale s{drawable->getCenter(), currentScale()};
                drawable->draw();
                /* drawable->drawWithoutAnimate(); */
            }
        }
};

/**
 * Animation played when an object is moved from a to b
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 * @param start begining point
 * @param end finishing point
 */
class MoveAnimation : public Animation
{
    private:
        Point start;
        Point end;
        Point currentTranslation() const
        {
            if (!isComplete())
                return (end-start)/duration * elapsed;
            else
                return {0, 0};
        }
    public:
        MoveAnimation(int duration, Point start, Point end, std::shared_ptr<Shape> drawable=nullptr) noexcept
            : Animation{duration, drawable}, start{start}, end{end} { }

        void draw() override
        {
            if (drawable) {
                ++elapsed;
                Translation t{ currentTranslation() };
                /* drawable->drawWithoutAnimate(); */
                drawable->draw();
            }
        }
};

#endif

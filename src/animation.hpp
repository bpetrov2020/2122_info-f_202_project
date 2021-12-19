#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <FL/fl_draw.H>
#include <iostream>
#include <memory>

#include "point.hpp"
#include "shape.hpp"

class AnimatableShape;

/*----------------------------------------------------------
 * Transformations
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
 * Animations
 *--------------------------------------------------------*/

enum class AnimationT
{
    StillAnimation
    , ScaleAnimation
    , MoveAnimation
    , PulseAnimation
};

/**
 * Base class of all animations
 *
 * The drawable can be nullptr so to be able to create
 * animations and tie them to drawables after their
 * creation. The timers will start decreasing the moment
 * the animations are tied.
 */
class Animation
{
    protected:
        std::shared_ptr<AnimatableShape> drawable;
        int elapsed = 0;
        int duration;
    public:
        Animation(int duration, std::shared_ptr<AnimatableShape> drawable = nullptr) noexcept
            : drawable{drawable}, duration{duration} { }

        Animation(const Animation& a) = delete;
        Animation operator=(const Animation& a) = delete;

        virtual ~Animation() noexcept = default;

        virtual void draw() = 0;

        bool isComplete() const
        {
            return elapsed>duration;
        }

        void attachTo(std::shared_ptr<AnimatableShape> drawable_)
        {
            drawable = drawable_;
        }

        virtual AnimationT type() const = 0;
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
        StillAnimation(int duration, std::shared_ptr<AnimatableShape> drawable = nullptr) noexcept
            : Animation{duration, drawable} { }

        void draw() override;
        AnimationT type() const override
        {
            return AnimationT::StillAnimation;
        }
};

/**
 * Animation played when something is cleared (e.g. a cell)
 *
 * @param drawable pointer to the drawable that has the animation
 * @param duration time during which the animation is active
 */
class ScaleAnimation : public Animation
{
    private:
        double currentScale() const;
    public:
        ScaleAnimation(int duration, std::shared_ptr<AnimatableShape> drawable = nullptr) noexcept
            : Animation{duration, drawable} { }

        void draw() override;
        AnimationT type() const override { return AnimationT::ScaleAnimation; }
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
        Point currentTranslation() const;
    public:
        MoveAnimation(int duration, Point start, Point end, std::shared_ptr<AnimatableShape> drawable=nullptr) noexcept
            : Animation{duration, drawable}, start{start}, end{end} { }

        void draw() override;
        AnimationT type() const override { return AnimationT::MoveAnimation; }

        Point getStart() const { return start; }
        Point getEnd() const { return end; }
};

/**
 * Pulse animation, for suggested moves
 */
class PulseAnimation : public Animation
{
    private:
        double currentScale();
    public:
        PulseAnimation(int duration, std::shared_ptr<AnimatableShape> drawable=nullptr) noexcept
            : Animation{duration, drawable}
        {
        }

        void draw() override;
        AnimationT type() const override { return AnimationT::PulseAnimation; }
};

#endif

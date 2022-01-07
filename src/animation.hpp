#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "point.hpp"
#include "shape.hpp"

#include <FL/fl_draw.H>

#include <iostream>
#include <memory>

class AnimatableShape;

/*----------------------------------------------------------
 * Transformations
 *--------------------------------------------------------*/

/**
 * structure that will instantiate a translation.
 */
struct Translation {
    Translation(Point p) {
        fl_push_matrix();
        fl_translate(p.x, p.y);
    }
    ~Translation() {
        fl_pop_matrix();
    }
};

/**
 * structure that will instantiate a scaling.
 */
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

/**
 * structure that will instantiate a rotation.
 */
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

/// enumerates the different possible animations.
enum class AnimationT
{
    StillAnimation
    , ClearAnimation
    , MoveAnimation
    , HintAnimation
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
        std::shared_ptr<AnimatableShape> m_drawable;
        int m_elapsed {0};
        int m_duration;
    public:
        Animation(int duration, std::shared_ptr<AnimatableShape> drawable = nullptr) noexcept
            : m_drawable{drawable}, m_duration{duration} { }

        Animation(const Animation& a) = delete;
        Animation operator=(const Animation& a) = delete;

        virtual ~Animation() noexcept = default;

        virtual void draw() = 0;

        bool isComplete() const
        {
            return m_elapsed > m_duration;
        }

        bool isAttachedTo() const
        {
            return static_cast<bool>(m_drawable);
        }

        void attachTo(std::shared_ptr<AnimatableShape> drawable)
        {
            m_drawable = drawable;
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
class ClearAnimation : public Animation
{
    private:
        double currentScale() const;
    public:
        ClearAnimation(int duration, std::shared_ptr<AnimatableShape> drawable = nullptr) noexcept
            : Animation{duration, drawable} { }

        void draw() override;
        AnimationT type() const override { return AnimationT::ClearAnimation; }
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
        Point m_start;
        Point m_end;
        Point currentTranslation() const;
    public:
        MoveAnimation(int duration, Point start, Point end, std::shared_ptr<AnimatableShape> drawable=nullptr) noexcept
            : Animation{duration, drawable}, m_start{start}, m_end{end} { }

        void draw() override;
        AnimationT type() const override { return AnimationT::MoveAnimation; }

        Point getStart() const { return m_start; }
        Point getEnd() const { return m_end; }
};

/**
 * Pulse animation, for hinted moves
 */
class HintAnimation : public Animation
{
    private:
        double currentScale();
    public:
        HintAnimation(int duration, std::shared_ptr<AnimatableShape> drawable=nullptr) noexcept
            : Animation{duration, drawable}
        {
        }

        void draw() override;
        AnimationT type() const override { return AnimationT::HintAnimation; }
};

#endif

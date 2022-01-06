#ifndef COMMON_HPP
#define COMMON_HPP

#include <memory>
#include <cassert>

#include "animation.hpp"

class Shape;    // inclusion not needed, this is faster

// Constants
const int ANIM_TIME = 10;

/**
  Part of the program the user can interact with
  */
struct Interactive
{
    virtual ~Interactive() noexcept = default;

    virtual void mouseMove(Point) = 0;
    virtual void mouseClick(Point) = 0;
    virtual void mouseDrag(Point) = 0;
};

/**
  Part of the program that is drawn on the screen

  Can have an animation

  @param shape shape to be contained in the object
  */
class DrawableContainer
{
    protected:
        std::shared_ptr<Shape> drawable;
        std::shared_ptr<Animation> animation;
    public:
        DrawableContainer(std::shared_ptr<Shape> shape) noexcept
            : drawable{shape}, animation{nullptr} { }
        virtual ~DrawableContainer() noexcept = default;

        // Center
        Point getCenter() const { return drawable->getCenter(); }
        void setCenter(const Point& p) { drawable->setCenter(p); }

        /// Draws the content
        virtual void draw() {
            if (hasAnimation())
                animation->draw();
            else
                drawable->draw();

            if (hasAnimation() && animation->isComplete()) {
                animationFinished(animation->type());
                removeAnimation();
            }
        }

        virtual bool hasAnimation() { return static_cast<bool>(animation); }

        virtual void addAnimation(std::shared_ptr<Animation> newanim)
        {
            assert(std::dynamic_pointer_cast<AnimatableShape>(drawable));
            assert(!hasAnimation());

            removeAnimation();
            newanim->attachTo(std::dynamic_pointer_cast<AnimatableShape>(drawable));
            animation = std::move(newanim);
        }

        // Function called at end of an animation with the type of it
        virtual void removeAnimation() { animation.reset(); }
        virtual void animationFinished(AnimationT) { }
};

enum class Direction {
    South,
    North,
    West,
    East,
    SouthWest,
    SouthEast,
    NorthWest,
    NorthEast
};

constexpr std::array<Point, 8> DirectionModifier {
    Point{ 0, -1},  // South
    Point{ 0,  1},  // North
    Point{-1,  0},  // West
    Point{ 1,  0},  // East
    Point{-1, -1},  // SouthWest
    Point{ 1, -1},  // SouthEast
    Point{-1,  1},  // NorthWest
    Point{ 1,  1}   // NorthEast
};

#endif

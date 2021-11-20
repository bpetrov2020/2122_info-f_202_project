#ifndef COMMON_HPP
#define COMMON_HPP

/* #include <ostream> */
#include <memory>
#include <cassert>

#include "shape.hpp"
#include "animation.hpp"

// Constants
const int ANIM_TIME = 15;

/**
  Part of the program the user can interact with
  */
struct Interactive
{
    virtual ~Interactive() noexcept = default;

    virtual void mouseMove(Point mouseLoc) = 0;
    virtual void mouseClick(Point mouseLoc) = 0;
    virtual void mouseDrag(Point mouseLoc) = 0;
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
            if (animation)
                animation->draw();
            else
                drawable->draw();
            if (animation && animation->isComplete()) {
                animationFinished(animation->type());
                animation.reset();
            }
        }

        virtual bool hasAnimation() { return static_cast<bool>(animation); }

        virtual void addAnimation(std::shared_ptr<Animation> newanim)
        {
            assert(std::dynamic_pointer_cast<AnimatableShape>(drawable));
            assert(!hasAnimation());
            /* if (animation) throw std::runtime_error("Animation not empty"); */
            animation.reset();
            newanim->attachTo(std::dynamic_pointer_cast<AnimatableShape>(drawable));
            animation = std::move(newanim);
        }

        // Function called at end of an animation with the type of it
        virtual void animationFinished(AnimationT a) { }
};

#endif

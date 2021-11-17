#ifndef COMMON_HPP
#define COMMON_HPP

/* #include "shape.hpp" */
/* #include "animation.hpp" */

/* #include <ostream> */
/* #include <memory> */

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
  */
class DrawableContainer
{
    protected:
        std::shared_ptr<Shape> drawable;
    public:
        DrawableContainer(std::shared_ptr<Shape> s) noexcept
            : drawable{s} { }
        virtual ~DrawableContainer() noexcept = default;

        Point getCenter() const { return drawable->getCenter(); }
        void setCenter(const Point& p) { drawable->setCenter(p); }

        /// Draws the content
        virtual void draw() { drawable->draw(); }

        /* virtual bool hasAnimation() { return drawable->hasAnimation(); } */
};

#endif

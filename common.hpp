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
/* class DrawableContainer : public Interactive */
/* { */
/*     protected: */
/*         std::shared_ptr<Shape> drawable; */
/*         std::shared_ptr<Animation> animation; */
/*     public: */
/*         DrawableContainer(std::shared_ptr<Shape> s) noexcept */
/*             : drawable{s}, animation{nullptr} { } */

/*         void mouseMove(Point mouseLoc) override { mouseLoc;} */
/*         void mouseClick(Point mouseLoc) override { mouseLoc;} */
/*         void mouseDrag(Point mouseLoc) override { mouseLoc;} */

/*         Point getCenter() const { return drawable->getCenter(); } */
/*         void setCenter(const Point& p) { drawable->setCenter(p); } */

/*         /// Draws the content with the animation (if available) */
/*         virtual void draw() */
/*         { */
/*             if (animation && animation->isComplete()) animation.reset(); */
/*             if (animation) animation->draw(); */
/*             else drawable->draw(); */
/*         } */

/*         virtual void addAnimation(std::shared_ptr<Animation> new_anim) */
/*         { */
/*             removeAnimation(); */
/*             new_anim->attachTo(drawable); */
/*             animation = new_anim; */
/*         } */

/*         virtual void removeAnimation() { animation.reset(); } */
/*         virtual bool hasAnimation() { return static_cast<bool>(animation); } */
/* }; */
/// Draws the content without the animation
/* virtual void drawWithoutAnimate() */
/* { */
/*     drawable.draw(); */
/* } */

#endif

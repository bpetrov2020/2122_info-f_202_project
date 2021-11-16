#include "shape.hpp"

Shape::Shape(
        Point center,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        center{center},
        fillColor{fillColor},
        frameColor{frameColor}
{}

Rectangle::Rectangle(
        Point center,
        int width,
        int height,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        Shape{center, fillColor, frameColor},
        width{width},
        height{height}
{}

Shape::~Shape() {}

void Rectangle::draw() const
{
    fl_draw_box(
            FL_FLAT_BOX,
            center.x-width/2,
            center.y-height/2,
            width,
            height,
            fillColor
            );
    fl_draw_box(
            FL_BORDER_FRAME,
            center.x-width/2,
            center.y-height/2,
            width,
            height,
            frameColor
            );
}

bool Rectangle::contains(Point p) const
{
    return p.x >= center.x - width/2 &&
        p.x < center.x + width/2 &&
        p.y >= center.y - height/2 &&
        p.y < center.y + height/2;
}

#include "shape.hpp"

/*----------------------------------------------------------
                        Shape
----------------------------------------------------------*/

Shape::Shape(Point center)
    : center{center} { }

Point Shape::getCenter() const
{
    return center;
}

void Shape::setCenter(const Point& p)
{
    center = p;
}

/*----------------------------------------------------------
                        Rectangle
----------------------------------------------------------*/

Rectangle::Rectangle(Point center, int width, int height, Fl_Color fillColor, Fl_Color frameColor)
    : Shape{center}, width{width}, height{height}, fillColor{fillColor}, frameColor{frameColor} {}

void Rectangle::draw()
{
    std::cerr << "between" << std::endl;
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

bool Rectangle::contains(const Point& p) const
{
    return p.x >= center.x - width/2 &&
        p.x < center.x + width/2 &&
        p.y >= center.y - height/2 &&
        p.y < center.y + height/2;
}

auto Rectangle::getWidth() const
{
    return width;
}

void Rectangle::setWidth(int w)
{
    width = w;
}

auto Rectangle::getHeight() const
{
    return height;
}

void Rectangle::setHeight(int h)
{
    height = h;
}

auto Rectangle::getFillColor() const
{
    return fillColor;
}

void Rectangle::setFillColor(const Fl_Color& c)
{
    fillColor = c;
}

auto Rectangle::getFrameColor() const
{
    return frameColor;
}

void Rectangle::setFrameColor(const Fl_Color& c)
{
    frameColor = c;
}

/*----------------------------------------------------------
                        Text
----------------------------------------------------------*/

Text::Text(Point center, std::string str, int fontSize, Fl_Color color)
    : Shape{center}, str{str}, fontSize{fontSize}, color{color} {}

void Text::draw()
{
    fl_color(color);
    fl_font(FL_HELVETICA, fontSize);
    int width, height;
    fl_measure(str.c_str(), width, height, false);
    fl_draw(str.c_str(), center.x-width/2, center.y-fl_descent()+height/2);
}

bool Text::contains(const Point& p) const
{
    int width, height;
    fl_measure(str.c_str(), width, height, false);

    return p.x >= center.x - width/2 &&
        p.x < center.x + width/2 &&
        p.y >= center.y - height/2 &&
        p.y < center.y + height/2;
}

std::string Text::getString() {
    return str;
}

void Text::setString(const std::string &newString) {
    str = newString;
}

int Text::getFontSize() {
    return fontSize;
}

void Text::setFontSize(int newFontSize) {
    fontSize = newFontSize;
}

auto Text::getColor() const
{
    return color;
}

void Text::setColor(const Fl_Color& c)
{
    color = c;
}



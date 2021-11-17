#include "shape.hpp"

/*----------------------------------------------------------
 * Rectangle
 *--------------------------------------------------------*/

Rectangle::Rectangle(
        Point center,
        int width,
        int height,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        AnimatableShape{center},
        width{width},
        height{height},
        fillColor{fillColor},
        frameColor{frameColor}
{}

void Rectangle::draw()
{
    std::array<Point, 5> points {
        Point{center.x - width/2, center.y - height/2},
        Point{center.x - width/2, center.y + height/2},
        Point{center.x + width/2, center.y + height/2},
        Point{center.x + width/2, center.y - height/2},
        Point{center.x - width/2, center.y - height/2}
    };

    // Fill
    fl_color(fillColor);
    fl_begin_polygon();
    for (auto &point : points) {
        fl_vertex(point.x, point.y);
    }
    fl_end_polygon();

    // Frame
    fl_color(frameColor);
    fl_begin_line();
    for (auto &point : points) {
        fl_vertex(point.x, point.y);
    }
    fl_end_line();
}

bool Rectangle::contains(const Point& p) const
{
    return p.x >= center.x - width/2 &&
        p.x < center.x + width/2 &&
        p.y >= center.y - height/2 &&
        p.y < center.y + height/2;
}

/*----------------------------------------------------------
 * Circle
 * -------------------------------------------------------*/

Circle::Circle(
        Point center,
        int radius,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        AnimatableShape{center},
        radius{radius},
        fillColor{fillColor},
        frameColor{frameColor}
{}

void Circle::draw()
{
    std::array<Point,37> points;
    for (int i=0;i<36;i++)
        points[i]={static_cast<int>(center.x+radius*std::sin(i*10*std::numbers::pi/180)),
            static_cast<int>(center.y+radius*std::cos(i*10*std::numbers::pi/180))};
    points[36]=points[0];

    fl_color(fillColor);
    fl_begin_polygon();
    for (auto& point : points) {
        fl_vertex(point.x, point.y);
    }
    fl_end_polygon();

    fl_color(frameColor);
    fl_begin_line();
    for (auto& point : points) {
        fl_vertex(point.x, point.y);
    }
    fl_end_line();
}

bool Circle::contains(const Point& p) const
{
    return ( (p.x-center.x)*(p.x-center.x)
            +(p.y-center.y)*(p.y-center.y)<=radius*radius);
}

/*----------------------------------------------------------
 * Text
 *--------------------------------------------------------*/

Text::Text(
        Point center,
        std::string str,
        int fontSize,
        Fl_Color color
        )
    :
        Shape{center},
        str{str},
        fontSize{fontSize},
        color{color}
{}

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

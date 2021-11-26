#include "shape.hpp"

/*----------------------------------------------------------
 * Rectangle
 *--------------------------------------------------------*/

Shape::Shape(
        Point center,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        center{center},
        fillColor{fillColor},
        frameColor{frameColor}
{ }

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
        AnimatableShape{center, fillColor, frameColor},
        width{width},
        height{height}
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
 * StripedRectangle
 *--------------------------------------------------------*/

StripedRectangle::StripedRectangle(
        Point center,
        int width,
        int height,
        Axis axis,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        Rectangle{center, width, height, fillColor, frameColor},
        axis{axis}
{}

void StripedRectangle::draw()
{
    Rectangle::draw();
    std::array<Point, 6> pointsStrip;

    if (axis == Axis::Vertical) {
        pointsStrip = {
            Point{getCenter().x - width/4, getCenter().y - height/2},
            Point{getCenter().x - width/4, getCenter().y + height/2},
            Point{getCenter().x, getCenter().y + height/2},
            Point{getCenter().x, getCenter().y - height/2},
            Point{getCenter().x + width/4, getCenter().y - height/2},
            Point{getCenter().x + width/4, getCenter().y + height/2},
        };
    } else if (axis == Axis::Horizontal) {
        pointsStrip = {
            Point{getCenter().x - width/2, getCenter().y - height/4},
            Point{getCenter().x + width/2, getCenter().y - height/4},
            Point{getCenter().x + width/2, getCenter().y},
            Point{getCenter().x - width/2, getCenter().y},
            Point{getCenter().x - width/2, getCenter().y + height/4},
            Point{getCenter().x + width/2, getCenter().y + height/4}
        };
    }

    fl_color(FL_WHITE);
    fl_line_style(0, width/8);

    Point p;
    for (int i=0; i<3; ++i) {
        fl_begin_line();
        p = pointsStrip.at(2*i);
        fl_vertex(p.x, p.y);
        p = pointsStrip.at(2*i + 1);
        fl_vertex(p.x, p.y);
        fl_end_line();
    }

    fl_line_style(0); // back to default

    /* std::array<Point, 5> points { */
    /*     Point{center.x - width/2, center.y - height/2}, */
    /*         Point{center.x - width/2, center.y + height/2}, */
    /*         Point{center.x + width/2, center.y + height/2}, */
    /*         Point{center.x + width/2, center.y - height/2}, */
    /*         Point{center.x - width/2, center.y - height/2} */
    /* }; */

    /* // Frame */
    /* fl_color(frameColor); */
    /* fl_begin_line(); */
    /* for (auto &point : points) { */
    /*     fl_vertex(point.x, point.y); */
    /* } */
    /* fl_end_line(); */
}

/*----------------------------------------------------------
 * Star
 *--------------------------------------------------------*/

Star::Star(
        Point center,
        int width,
        int height,
        Fl_Color fillColor,
        Fl_Color frameColor
        )
    :
        Rectangle{center, width, height, fillColor, frameColor}
{}

void Star::draw()
{
    Rectangle::draw();
    std::array<Point, 5> pointsStar {
            Point{center.x - width/1.5, center.y},
            Point{center.x, center.y + height/1.5},
            Point{center.x + width/1.5, center.y},
            Point{center.x, center.y - height/1.5},
            Point{center.x - width/1.5, center.y}
    };

    // Fill
    fl_color(fillColor);
    fl_begin_polygon();
    for (auto &point : pointsStar) {
        fl_vertex(point.x, point.y);
    }
    fl_end_polygon();

    // Frame
    fl_color(frameColor);
    fl_begin_line();
    for (auto &point : pointsStar) {
        fl_vertex(point.x, point.y);
    }
    fl_end_line();
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
        AnimatableShape{center, fillColor, frameColor},
        radius{radius}
{}

void Circle::draw()
{
    std::array<Point,37> points;
    for (int i=0; i<36; i++)
        points[static_cast<unsigned>(i)] = {static_cast<int>(center.x+radius*std::sin(i*10*std::numbers::pi/180)),
            static_cast<int>(center.y+radius*std::cos(i*10*std::numbers::pi/180))};
    points[36]=points[0];

    // Fill
    fl_color(fillColor);
    fl_begin_polygon();
    for (auto& point : points) {
        fl_vertex(point.x, point.y);
    }
    fl_end_polygon();

    // Frame
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
        Shape{center, color},
        str{str},
        fontSize{fontSize}
{}

void Text::draw()
{
    fl_color(fillColor);
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

#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <Fl/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include <array>
#include <cmath>
#include <memory>

#include "point.hpp"
#include "animation.hpp"

class Animation;

/**
 * Base class of all shapes
 */
class Shape
{
    protected:
        Point center;
        Fl_Color fillColor;
        Fl_Color frameColor;
    public:
        Shape(Point center, Fl_Color fillColor = FL_WHITE, Fl_Color frameColor = FL_BLACK);
        virtual ~Shape() noexcept = default;

        virtual void draw() = 0;
        virtual bool contains(const Point& p) const = 0;

        // Center
        Point getCenter() const { return center; }
        void setCenter(Point p) { center = p; }

        // Fill Color
        Fl_Color getFillColor() const { return fillColor; }
        void setFillColor(const Fl_Color& c) { fillColor = c; }

        // Frame Color
        Fl_Color getFrameColor() const { return frameColor; }
        void setFrameColor(const Fl_Color& c) { frameColor = c; }
};

class AnimatableShape : public Shape
{
    protected:
        std::unique_ptr<Animation> animation;  // an animation can be tied to at most one object
    public:
        AnimatableShape(Point center, Fl_Color fillColor = FL_WHITE, Fl_Color frameColor = FL_BLACK)
            : Shape{center, fillColor, frameColor}, animation{nullptr} { }
};

/**
 * Rectangle class.
 *
 * Used to display a filled-in rectangle on the screen
 * with different colors for the fill and the border.
 */
class Rectangle : public AnimatableShape
{
    protected:
        int width;
        int height;
    public:
        Rectangle(
                Point center,
                int width,
                int height,
                Fl_Color fillColor = FL_WHITE,
                Fl_Color frameColor = FL_BLACK
                );

        void draw() override;
        bool contains(const Point& p) const override;

        int getWidth() const { return width; }
        void setWidth(int w) { width = w; }

        int getHeight() const { return height; }
        void setHeight(int h) { height = h; }
};

enum class Axis
{
    Vertical,
    Horizontal
};

/**
 * Striped rectangle
 */
class StripedRectangle : public Rectangle
{
    protected:
        Axis axis;
    public:
        StripedRectangle(
                Point center,
                int width,
                int height,
                Axis axis = Axis::Horizontal,
                Fl_Color fillColor = FL_WHITE,
                Fl_Color frameColor = FL_BLACK
                );

        void draw() override;

        Axis getAxis() const { return axis; }
        void setAxis(Axis newdir) { axis = newdir; }
};

/**
 * Star shape for WrappedCandie
 */
class Star : public Rectangle
{
public:
    Star(
            Point center,
            int width,
            int height,
            Fl_Color fillColor = FL_WHITE,
            Fl_Color frameColor = FL_BLACK
    );

    void draw() override;
};

/**
 * Circle shape
 *
 * Used to display a filled-in circle on the screen
 * with different colors for the fill and the border.
 */
class Circle : public AnimatableShape
{
    private:
        int radius;
    public:
        Circle(
                Point center,
                int radius,
                Fl_Color fillColor = FL_WHITE,
                Fl_Color frameColor = FL_BLACK
                );

        void draw() override;
        bool contains(const Point& p) const override;

        int getRadius() const { return radius; }
        void setRadius(int r) { radius = r; }
};

/**
 * Text class
 *
 * Allows to display text on screen. The color, font size can
 * be configured.
 */
class Text : public Shape
{
    private:
        std::string str;
        int fontSize;
    public:
        Text(Point center, std::string s, int fontSize = 10, Fl_Color color = FL_BLACK);

        void draw() override;
        bool contains(const Point& p) const override;

        std::string getString() { return str; }
        void setString(const std::string& newString) { str = newString; }

        int getFontSize() { return fontSize; }
        void setFontSize(int newFontSize) { fontSize = newFontSize; }
};

#endif

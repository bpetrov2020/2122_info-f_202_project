#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <Fl/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

#include "point.hpp"

/**
 * Base class of all shapes
 */
class Shape
{
    protected:
        Point center;
    public:
        Shape(Point center)
            : center{center} { }
        virtual ~Shape() noexcept = default;

        virtual void draw() = 0;
        virtual bool contains(const Point& p) const = 0;

        // Center
        Point getCenter() const { return center; }
        void setCenter(Point p) { center = p; }
};

/**
 * Rectangle class.
 *
 * Used to display a filled-in rectangle on the screen
 * with different colors for the fill and the border.
 */
class Rectangle : public Shape
{
    private:
        int width;
        int height;
        Fl_Color fillColor;
        Fl_Color frameColor;
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

        Fl_Color getFillColor() const { return fillColor; }
        void setFillColor(const Fl_Color& c) { fillColor = c; }

        Fl_Color getFrameColor() const { return frameColor; }
        void setFrameColor(const Fl_Color& c) { frameColor = c; }
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
        Fl_Color color;
    public:
        Text(Point center, std::string s, int fontSize = 10, Fl_Color color = FL_BLACK);

        void draw() override;
        bool contains(const Point& p) const override;

        std::string getString() { return str; }
        void setString(const std::string& newString) { str = newString; }

        int getFontSize() { return fontSize; }
        void setFontSize(int newFontSize) { fontSize = newFontSize; }

        auto getColor() const { return color; }
        void setColor(const Fl_Color& c) { color = c; }
};

#endif
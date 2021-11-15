#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "point.hpp"

#include <Fl/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>

/**
    Base class of all shapes
*/
class Shape
{
    protected:
        Point center;
    public:
        Shape(Point center);
        virtual ~Shape() noexcept = default;

        virtual void draw() = 0;
        virtual bool contains(const Point& p) const = 0;

        // Center
        auto getCenter() const;
        void setCenter(const Point& p);
};

/**
    Rectangle class.

    Used to display a filled-in rectangle on the screen
    with different colors for the fill and the border.
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

        // Getters
        auto getWidth() const;
        void setWidth(int w);

        // Setters
        auto getHeight() const;
        void setHeight(int h);

        // FillColor
        auto getFillColor() const;
        void setFillColor(const Fl_Color& c);

        // Setters
        auto getFrameColor() const;
        void setFrameColor(const Fl_Color& c);
};

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

        std::string getString();
        void setString(const std::string& newString);

        int getFontSize();
        void setFontSize(int newFontSize);

        auto getColor() const;
        void setColor(const Fl_Color& c);

};

#endif

/* #include <Fl/Fl.H> */
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include "point.h"

#ifndef SHAPECLASS
#define SHAPECLASS

class Shape
{
    protected:
        Point center;
        Fl_Color fillColor;
        Fl_Color frameColor;
    public:
        Shape(
                Point center,
                Fl_Color fillColor = FL_WHITE,
                Fl_Color frameColor = FL_BLACK
             );
        virtual ~Shape();

        virtual void draw() const = 0;
        virtual bool contains(Point p) const = 0;

        // Getters
        auto getCenter() const { return center; }
        auto getFillColor() const { return fillColor; }
        auto getFrameColor() const { return frameColor; }

        // Setters
        void setCenter(Point p) { center = p; }
        void setFillColor(Fl_Color c) { fillColor = c; }
        void setFrameColor(Fl_Color c) { frameColor = c; }
};

#endif

/*--------------------------------------------------

  Rectangle class.

  Use to display a filled-in rectangle on the screen
  with different colors for the fill and the border.

  --------------------------------------------------*/

#ifndef RECTANGLECLASS
#define RECTANGLECLASS

class Rectangle : public Shape
{
    private:
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
        virtual ~Rectangle() = default;

        virtual void draw() const override;
        virtual bool contains(Point p) const override;

        // Getters
        auto getWidth() const { return width; }
        auto getHeight() const { return height; }

        // Setters
        void setWidth(int w) { width = w; }
        void setHeight(int h) { height = h; }
};

#endif

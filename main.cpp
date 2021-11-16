#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <string>
#include <math.h>
#include <time.h>
#include <chrono>
#include <vector>
#include <iostream>
#include <random>
#include <array>

#include "grid.h"
/* #include "cell_object.h" */

const int windowWidth = 700;
const int windowHeight = 700;
const int refreshPerSecond = 60;

/*--------------------------------------------------
                MainWindow class
--------------------------------------------------*/

class MainWindow : public Fl_Window
{
private:
        Grid canvas;
public:
    MainWindow()
        : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush"),
        canvas{{windowWidth/2, windowHeight/2}, windowWidth, windowHeight, 10}
    {
        Fl::add_timeout(1.0/refreshPerSecond, Timer_CB, this);
        resizable(this);
    }
    void draw() override
    {
        Fl_Window::draw();
        canvas.draw();
    }
    int handle(int event) override
    {
        switch (event) {
        case FL_MOVE:
            canvas.mouseMove(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        case FL_PUSH:
            canvas.mouseClick(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        case FL_DRAG:
            canvas.mouseDrag(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        /* case FL_KEYDOWN: */
        /*     canvas.keyPressed(Fl::event_key()); */
        /*     return 1; */
        }
        return 0;
    }
    static void Timer_CB(void *userdata)
    {
        MainWindow *o = (MainWindow*) userdata;
        o->redraw();
        Fl::repeat_timeout(1.0/refreshPerSecond, Timer_CB, userdata);
    }
};


/*--------------------------------------------------

                        Main

--------------------------------------------------*/


int main(int argc, char *argv[])
{
    srand(time(0));
    MainWindow window;
    window.show(argc, argv);
    return Fl::run();
}

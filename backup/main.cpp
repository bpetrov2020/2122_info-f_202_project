#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
/* #include <string> */
/* #include <math.h> */
/* #include <chrono> */
/* #include <random> */
/* #include <array> */

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <time.h>

#include "game.hpp"
#include "animation.hpp"
#include "shape.hpp"

const int windowWidth = 480;
const int windowHeight = 800;
const int refreshPerSecond = 60;

/*--------------------------------------------------
                MainWindow class
--------------------------------------------------*/

class MainWindow : public Fl_Window
{
    private:
        Game game;
        Rectangle a{Point{500, 500}, 600, 400};
    public:
        MainWindow()
            : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush"), game{*this}
        {
            Fl::add_timeout(1.0/refreshPerSecond, Timer_CB, this);
            resizable(this);
        }

        void draw() override
        {
            Fl_Window::draw();
            /* game.draw(); */
            Scale s{Point{50, 50}, 0.5};
            a.draw();
        }

        int handle(int event) override
        {
            switch (event) {
                case FL_MOVE:
                    game.mouseMove(Point{Fl::event_x(), Fl::event_y()});
                    return 1;
                case FL_PUSH:
                    game.mouseClick(Point{Fl::event_x(), Fl::event_y()});
                    return 1;
                case FL_DRAG:
                    game.mouseDrag(Point{Fl::event_x(), Fl::event_y()});
                    return 1;
                    /* case FL_KEYDOWN: */
                    /*     game.keyPressed(Fl::event_key()); */
                    /*     return 1; */
            }
            return 0;
        }

        static void Timer_CB(void *userdata)
        {
            MainWindow *o = static_cast<MainWindow*>(userdata);
            o->redraw();
            Fl::repeat_timeout(1.0/refreshPerSecond, Timer_CB, userdata);
        }
};


/*--------------------------------------------------
                      Main
--------------------------------------------------*/

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));
    MainWindow window;
    window.show(argc, argv);
    return Fl::run();
}

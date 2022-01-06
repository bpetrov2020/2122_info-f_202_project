#include "game.hpp"

#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Double_Window.H>

#include <time.h>

const int windowWidth = 500;
const int windowHeight = 600;
const int refreshPerSecond = 60;

/*--------------------------------------------------
 * MainWindow class
 *------------------------------------------------*/

class MainWindow : public Fl_Window
{
private:
        Game m_game;
public:
    MainWindow()
        : Fl_Window(500, 500, windowWidth, windowHeight, "Candy Crush"),
        m_game{*this}
    {
        Fl::add_timeout(1.0/refreshPerSecond, Timer_CB, this);
        /* resizable(this); */
    }

    void draw() override
    {
        Fl_Window::draw();
        m_game.draw();
    }

    int handle(int event) override
    {
        switch (event) {
        case FL_MOVE:
            m_game.mouseMove(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        case FL_PUSH:
            m_game.mouseClick(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        case FL_DRAG:
            m_game.mouseDrag(Point{Fl::event_x(), Fl::event_y()});
            return 1;
        /* case FL_KEYDOWN: */
        /*     m_game.keyPressed(Fl::event_key()); */
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
 * Main
 *------------------------------------------------*/

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned>(time(nullptr)));
    MainWindow window;
    window.show(argc, argv);
    return Fl::run();
}

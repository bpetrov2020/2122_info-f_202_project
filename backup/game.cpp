#include "game.hpp"

/*----------------------------------------------------------
 * View
 *--------------------------------------------------------*/

View::View(Fl_Window& win, Game& g)
    : DrawableContainer{
        std::make_shared<Rectangle>(
            Point{win.w()/2, win.h()/2},
                win.w(),
                win.h()
        )
    },
    window{win},
    game{g} { }

/*----------------------------------------------------------
 * Game
 *--------------------------------------------------------*/

Game::Game(Fl_Window& win)
    : window{win},
    view{std::make_shared<SplashScreen>(win, *this, "Alec & Boris", 15, 120)} { }

void Game::mouseMove(Point mouseLoc) { if(view) view->mouseMove(mouseLoc); }
void Game::mouseClick(Point mouseLoc) { if(view) view->mouseClick(mouseLoc); }
void Game::mouseDrag(Point mouseLoc) { if(view) view->mouseDrag(mouseLoc); }

void Game::draw() { if(view) view->draw(); }

void Game::loadView(std::shared_ptr<View> v)
{
    view.reset();
    view = std::move(v);
}

/*----------------------------------------------------------
 * SplashScreen
 *--------------------------------------------------------*/

SplashScreen::SplashScreen(
        Fl_Window& win,
        Game& game,
        std::string authors,
        int fontSize,
        int duration
        )
    : View{win, game},
    author{std::make_shared<Text>(Point{win.w()/2, win.h()/2}, authors, 40)}
{
    addAnimation(std::make_shared<ClearAnimation>(duration));
}

void SplashScreen::draw()
{
    DrawableContainer::draw();  // draw the background
    author.draw();              // draw the author's name
    if (!animation)
        /* game.loadView(std::make_shared<LevelSelector>());  // TODO title screen */
        exit(1);
}

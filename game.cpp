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
    /* view{std::make_shared<SplashScreen>(win, *this, "Authors", 15, 120)} { } TODO */
    view{std::make_shared<Level>(win, *this)} { }


void Game::mouseMove(Point mouseLoc)  { if(view) view->mouseMove(mouseLoc); }
void Game::mouseClick(Point mouseLoc) { if(view) view->mouseClick(mouseLoc); }
void Game::mouseDrag(Point mouseLoc)  { if(view) view->mouseDrag(mouseLoc); }

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
    author{std::make_shared<Text>(Point{win.w()/2, win.h()/2}, authors, fontSize)}
{
    addAnimation(std::make_shared<StillAnimation>(duration));
}

void SplashScreen::draw()
{
    DrawableContainer::draw();  // draw the background
    author.draw();              // draw the author's name
    if (toBeReplaced)
        game.loadView(std::make_shared<Level>(window, game));
}

void SplashScreen::animationFinished(AnimationT animationType)
{
    switch (animationType) {
        case AnimationT::StillAnimation:
            toBeReplaced = true;
            break;
        case AnimationT::ScaleAnimation:
        case AnimationT::MoveAnimation:
            break;
    }
}

/*----------------------------------------------------------
 * Level
 *--------------------------------------------------------*/

Level::Level(Fl_Window& window, Game& game)
    : View{window, game},
    board{Point{window.w()/2, window.h()/2}, window.w(), window.h(), 6}
{
    //TODO init board with candies
    /* for (int i=0; i<board.cols(); ++i) { */
    /*     for (int j=0; j<board.rows(); ++j) { */

    /*     } */
    /* } */
}

void Level::draw()
{
    DrawableContainer::draw();
    board.draw();
}

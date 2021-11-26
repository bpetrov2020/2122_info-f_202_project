#ifndef GAME_H
#define GAME_H

#include <FL/Fl_Window.H>
#include <memory>
#include <iostream>

#include "common.hpp"
#include "shape.hpp"
#include "grid.hpp"

class Game;

/**
  Abstract wrapper of a screen in the game

  A view is a collection of elements
  that are to be shown at a given time in
  the game. Two views cannot be shown at
  the same time.

  Changing the view is done by the Game instance
  but should be asked by a View instance. A View
  should know when it's done and make a request for
  a view switching to the game.

  @param game game instance the view is tied to
  */
class View : public DrawableContainer, public Interactive
{
    protected:
        Fl_Window& window;
        Game& game;
    public:
        View(Fl_Window& window, Game& game);
};

/**
 * A game, holds the different views of the game
 */
class Game : public Interactive
{
    private:
        Fl_Window& window;
        std::shared_ptr<View> view;
    public:
        Game(Fl_Window& win);

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void draw();

        void loadView(std::shared_ptr<View> v);
};

/**
 * First screen shown to the player
 *
 * Lasts a few seconds before the main menu is loaded
 */
class SplashScreen : public View
{
    private:
        DrawableContainer author;
        bool toBeReplaced = false;  // Whether or not the next screen should be loaded
    public:
        SplashScreen(Fl_Window& window, Game& game, std::string author, int fontSize, int duration);

        // Mouse interactions are disabled
        void mouseMove(Point mouseLoc) override { }
        void mouseClick(Point mouseLoc) override { }
        void mouseDrag(Point mouseLoc) override { }

        void draw() override;

        void animationFinished(AnimationT a) override;
};

/**
 * A level in the game
 */
class Level : public View
{
    private:
        /* Grid status; */
        Grid board;
        /* bool isFileValid() const; */
        /* bool initFromFile() const; */
    public:
        /* Level(const std::string filename); */
        Level(Fl_Window& window, Game& game);

        // From Interactive
        void mouseMove(Point mouseLoc) override  { board.mouseMove(mouseLoc); }
        void mouseClick(Point mouseLoc) override { board.mouseClick(mouseLoc); }
        void mouseDrag(Point mouseLoc) override  { board.mouseDrag(mouseLoc); }

        // From Drawable
        void draw() override;

        /* std::vector< std::vector<Cell*> > combinationsFrom(const Point &p); */
        /* bool isInCombination(const Point &point); */
        /* void processCombinationFrom(const Point &point); */

        /* bool makeFall(const Point &p); */
        /* void fillGrid(); */
};

#endif

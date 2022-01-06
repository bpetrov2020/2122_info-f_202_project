#ifndef GAME_H
#define GAME_H

#include "common.hpp"
#include "grid.hpp"
#include "level_data.hpp"
#include "level_status.hpp"
#include "observer.hpp"
#include "shape.hpp"

#include <FL/Fl_Window.H>
#include <memory>

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
class View : public DrawableContainer, public Interactive, public Observer
{
    protected:
        Fl_Window& window;
        Game& game;
    public:
        View(Fl_Window& window, Game& game);

        int w() const { return window.w(); }
        int h() const { return window.h(); }
};

/**
 * A game, holds the different views of the game
 */
class Game : public Interactive
{
    private:
        Fl_Window& window;
        std::shared_ptr<View> view;

        int bestScore;
        void writeScore();
        int numberOfLevels{9};
    public:
        Game(Fl_Window& win);

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void draw();

        void loadView(std::shared_ptr<View> v);

        void updateScore(int);
        void resetScore();

        int getBestScore() const { return bestScore; }

        int getNumberOfLevels() const { return numberOfLevels; }
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
        void mouseMove(Point) override { }
        void mouseClick(Point) override { }
        void mouseDrag(Point) override { }

        void draw() override;
        void update(Event) override {}

        void animationFinished(AnimationT a) override;
};

/**
 * A level in the game
 */
class Level : public View, public Subject
{
    private:
        LevelData m_data;
        LevelStatus m_status;
        Grid m_board;
        std::shared_ptr<State> m_boardController {nullptr};

        inline int gridSide(Fl_Window &win);
    public:
        Level(Fl_Window& window, Game& game, int levelNumber);

        void mouseMove(Point mouseLoc)  override { m_boardController->mouseMove(mouseLoc); }
        void mouseClick(Point mouseLoc) override { m_boardController->mouseClick(mouseLoc); }
        void mouseDrag(Point mouseLoc)  override { m_boardController->mouseDrag(mouseLoc); }

        void draw() override;

        void setState(std::shared_ptr<State> state);
        void replayLevel();
        void playNextLevel();

        void update(Event event) override;
        void updateScore(int toAdd) { m_status.updateScore(toAdd); }

        int getColorRange() const { return m_data.getColorRange(); }
        StandardCandy::Color getRandomCandyColor() const { return static_cast<StandardCandy::Color>(std::rand()%getColorRange()); }
};

#endif

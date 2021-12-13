#ifndef GAME_H
#define GAME_H

#include <FL/Fl_Window.H>
#include <memory>
/* #include <iostream> */
/* #include <fstream> */
/* #include <sstream> */

#include "observer.hpp"
#include "common.hpp"
#include "shape.hpp"
#include "grid.hpp"
/* #include "level_goal.hpp" */
#include "level_status.hpp"
#include "level_data.hpp"

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
    public:
        Game(Fl_Window& win);

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void draw();

        void loadView(std::shared_ptr<View> v);
};

/**
 * The level selector view
 */

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

        void animationFinished(AnimationT a) override;
};

/**
 * Container for the data needed to initialize a level
 *
 * The data should be of the following format:
 *
 * <Category> <value(s)>
 *
 * The categories available are:
 * -   Size           "1"     int
 * [-] Wall           "1..*"  Point
 * [-] SingleIcing    "1..*"  Point
 * [-] DoubleIcing    "1..*"  Point
 *
 * Only the size is obligatory. And it should alway be the first one.
 */
/* class LevelData */
/* { */
/*     private: */
/*         std::string m_levelName; */

/*         int gridSize {-1}; */
/*         int colorRange {6}; */

/*         std::string goalType {}; */

/*         std::vector<Point> wallsPos{}; */
/*         std::vector<Point> singleIcingPos{}; */
/*         std::vector<Point> doubleIcingPos{}; */

/*         // Functions used to fill the data from a file */
/*         void extractDataFrom(std::string filename); */
/*         void processLine(std::string line); */
/*         void fillFrom(std::vector<Point> &vect, std::istream &is); */
/*     public: */
/*         LevelData(std::string filename); */

/*         std::string levelName() const { return m_levelName; } */

/*         int getGridSize() const { return gridSize; } */
/*         int getColorRange() const { return colorRange; } */

/*         const auto &getWallsPos() const { return wallsPos; } */
/*         const auto &getSingleIncingPos() const { return singleIcingPos; } */
/*         const auto &getDoubleIcingPos() const { return doubleIcingPos; } */
/* }; */

/**
 * A level in the game
 */
class Level : public View, public Subject, public Observer
{
    private:
        LevelData m_data;
        LevelStatus m_status;
        Grid m_board;
        std::shared_ptr<State> m_boardController {nullptr};

        inline int gridSide(Fl_Window &win);
    public:
        Level(Fl_Window& window, Game& game, const std::string &filename);

        void mouseMove(Point mouseLoc)  override { m_boardController->mouseMove(mouseLoc); }
        void mouseClick(Point mouseLoc) override { m_boardController->mouseClick(mouseLoc); }
        void mouseDrag(Point mouseLoc)  override { m_boardController->mouseDrag(mouseLoc); }

        void draw() override;

        void setState(std::shared_ptr<State> state);
        void replayLevel();
        void playNextLevel();

        void update(Event event) override;
        void updateScore(int toAdd) { m_status.updateScore(toAdd); }
};

#endif

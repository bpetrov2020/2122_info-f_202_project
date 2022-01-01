#include "game.hpp"

#include "ls_view.hpp"

#include <fstream>

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
    : window{win}
    , view{nullptr}//, "data/levels/level1.txt")}
    , bestScore {-1}
    /* view{std::make_shared<SplashScreen>(win, *this, "Authors", 15, 120)} { } TODO */
{
    std::ifstream scoreSrc {"data/best_score.txt"};
    if (scoreSrc) {
        scoreSrc >> bestScore;
        if (!scoreSrc) {
            throw std::runtime_error("Game::Game: Wrong score formatting");
        }
    }
    view = std::make_shared<LevelSelector>(win, *this);
}


void Game::mouseMove(Point mouseLoc)  { if(view) view->mouseMove(mouseLoc); }
void Game::mouseClick(Point mouseLoc) { if(view) view->mouseClick(mouseLoc); }
void Game::mouseDrag(Point mouseLoc)  { if(view) view->mouseDrag(mouseLoc); }

void Game::draw() { if(view) view->draw(); }

void Game::loadView(std::shared_ptr<View> v)
{
    view.reset();
    view = std::move(v);
}

void Game::writeScore()
{
    std::ofstream scoreDest {"data/best_score.txt"};
    scoreDest << bestScore;
}

void Game::updateScore(int newScore)
{
    if (newScore>bestScore)
        bestScore = newScore;

    writeScore();
}

void Game::resetScore()
{
    bestScore = 0;
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
    /* if (toBeReplaced) */
        /* game.loadView(std::make_shared<Level>(window, game, "data/levels/1.txt")); */
}

void SplashScreen::animationFinished(AnimationT animationType)
{
    switch (animationType) {
        case AnimationT::StillAnimation:
            toBeReplaced = true;
            break;
        default:
            break;
    }
}

/*----------------------------------------------------------
 * Level
 *--------------------------------------------------------*/

// TODO make adaptable to height
Level::Level(Fl_Window& window, Game& game, int levelNumber)
    : View{window, game},
    m_data{levelNumber},
    m_status{Point{window.w()/2, window.h()/12*11}, gridSide(window), gridSide(window)/5, m_data},
    m_board{Point{window.w()/2, window.h()/12*5}, gridSide(window), gridSide(window), m_data},
    m_boardController{nullptr}
{
    m_status.registerObserver(this);
    registerObserver(&m_status);

    setState(std::make_shared<GridInitState>(this, m_board, m_data));
}

inline int Level::gridSide(Fl_Window &win)
{
    return win.h() >= win.w() ? win.w() : win.h()/6*5;
}

void Level::draw()
{
    DrawableContainer::draw();  // background of the level
    m_board.draw();
    m_status.draw();
    m_boardController->draw();
}

void Level::setState(std::shared_ptr<State> state)
{
    m_boardController = state;
    m_board.setState(state);
}

void Level::replayLevel()
{
    game.loadView(std::make_shared<Level>(window, game, m_data.levelNumber()));
}

void Level::playNextLevel()
{
    if (m_data.levelNumber() < game.getNumberOfLevels())
        game.loadView(std::make_shared<Level>(window, game, m_data.levelNumber()+1));
    else
        game.loadView(std::make_shared<LevelSelector>(window, game));
}

void Level::update(Event event)
{
    switch (event) {
        case Event::GoalReached:
            game.updateScore(m_status.score());
            setState(std::make_shared<LevelPassedState>(this, m_board));
            break;
        case Event::NoMoreMoves:
            game.updateScore(m_status.score());
            setState(std::make_shared<LevelNotPassedState>(this, m_board));
            break;
        case Event::LevelPassed:
            playNextLevel();
            break;
        case Event::LevelNotPassed:
            replayLevel();
            break;
        default:
            m_status.update(event);
            break;
    }
}

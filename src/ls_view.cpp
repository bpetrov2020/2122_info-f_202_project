#include "ls_view.hpp"

#include "ls_state.hpp"

LevelSelector::LevelSelector(Fl_Window &win, Game &game)
    : View {win, game}
    , m_title {std::make_shared<Text>(Point {w()/2, h()/10}, "Square Crush Saga", 20)}
    , m_bestScore {std::make_shared<Text>(Point {w()/2, h()/10*9}, std::to_string(game.getBestScore()), 14)}
    , m_levels {Point {w()/2, h()/2}, 250, 250, 3, 3}
    , m_levelsController {std::make_shared<LevelSelectorState>(this, m_levels)}
{
    m_levels.setState(m_levelsController);
}

void LevelSelector::draw()
{
    View::draw();
    m_title.draw();
    m_bestScore.draw();
    m_levelsController->draw();
}

void LevelSelector::mouseMove(Point mouseLoc)
{
    m_levelsController->mouseMove(mouseLoc);
}

void LevelSelector::mouseClick(Point mouseLoc)
{
    m_levelsController->mouseClick(mouseLoc);
}

void LevelSelector::mouseDrag(Point mouseLoc)
{
    m_levelsController->mouseDrag(mouseLoc);
}

void LevelSelector::loadLevel(int levelNo)
{
    game.loadView(std::make_shared<Level>(window, game, levelNo));
}

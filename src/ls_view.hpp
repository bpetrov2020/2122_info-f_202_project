#pragma once

#include "game.hpp"

enum class Event;
class LevelSelectorState;

/**
 * Screen that allows us to select a level
 */
class LevelSelector : public View
{
public:
    LevelSelector(Fl_Window &, Game &);

    void draw() override;
    void mouseMove(Point) override;
    void mouseClick(Point) override;
    void mouseDrag(Point) override;

    void loadLevel(int);
    void update(Event) override {}

private:
    DrawableContainer m_title;
    DrawableContainer m_bestScoreLabel;
    DrawableContainer m_bestScore;

    Grid m_levels;
    std::shared_ptr<LevelSelectorState> m_levelsController {nullptr};
};

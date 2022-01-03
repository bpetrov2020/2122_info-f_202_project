#pragma once

#include "board_state.hpp"
#include "ls_view.hpp"

class Grid;
class Point;
enum class Event;

/**
 * LevelSelectorState,
 * State where we are able to select a level.
 *
 * @param levelSelector view of the level selector
 * @param grid the grid the state is tied to
 */
class LevelSelectorState : public Interactive, public State
{
public:
    LevelSelectorState(View *, Grid &);

    void draw();
    void mouseMove(Point) override;
    void mouseClick(Point) override;
    void mouseDrag(Point) override;

    void update(Event);

    void gridAnimationFinished(const Point &) {}

private:
    Grid m_grid;
    int m_levelToLoad {-1};

    void selectionChanged();
};

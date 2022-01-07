#include "level_status.hpp"

#include "observer.hpp"
#include "shape.hpp"
#include "level_data.hpp"

#include <memory>

// TODO make the thing cleaner
LevelStatus::LevelStatus(const Point &center, int width, int height, LevelData &data) noexcept
    : DrawableContainer {std::make_shared<Rectangle>(center, width, height)}
    , m_scoreLabelDrawable {Point {width/4, center.y - height/2 + height/3}, "Score", height/6, FL_BLACK}
    , m_scoreDrawable {Point {width/4, center.y - height/2 + height/3*2}, "0", height/5, FL_BLACK}
    , m_goal {data.goal()}
    , m_movesLeftLabelDrawable {Point {width/2, center.y - height/2 + height/3}, "Moves", height/6, FL_BLACK}
    , m_movesLeftDrawable {Point {width/2, center.y - height/2 + height/3*2}, "", height/5, FL_BLACK}
    , m_goalLabelDrawable {Point {width/4*3, center.y - height/2 + height/3}, data.goalTypeString(), height/6, FL_BLACK}
    , m_goalDrawable {Point {width/4*3, center.y - height/2 + height/3*2}, "", height/5, FL_BLACK}
{
    m_goal->registerObserver(this);
    registerObserver(m_goal.get());

    update(Event::GoalChanged);
}

void LevelStatus::updateScore(int toAdd)
{
    m_score += toAdd;
    m_scoreDrawable.setString(std::to_string(m_score));
}

int LevelStatus::scoreValueOf(Event event) const
{
    int value {0};

    switch (event) {
    case Event::CellCleared:
        value = 50;
        break;
    case Event::ThreeMatch:
        value = 50;
        break;
    case Event::StripedMatch:
        value = 125;
        break;
    case Event::WrappedMatch:
        value = 200;
        break;
    case Event::ColourBombMatch:
        value = 500;
        break;
    default:
        std::cerr << "No score value for this event";
        break;
    }

    return value;
}

void LevelStatus::update(Event event)
{
    switch (event) {
    case Event::GoalReached:
    case Event::NoMoreMoves:
        notifyObservers(event);
        break;
    case Event::GoalChanged:
        m_movesLeftDrawable.setString(std::to_string(m_goal->movesLeft()));
        m_goalDrawable.setString(m_goal->progressToString());
        break;
    case Event::CellCleared:
    case Event::ThreeMatch:
    case Event::StripedMatch:
    case Event::WrappedMatch:
    case Event::ColourBombMatch:
        updateScore(scoreValueOf(event));
        break;
    default:
        m_goal->update(event);
        break;
    }
}

void LevelStatus::draw()
{
    DrawableContainer::draw(); // background
    m_scoreLabelDrawable.draw();
    m_scoreDrawable.draw();
    m_movesLeftLabelDrawable.draw();
    m_movesLeftDrawable.draw();
    m_goalLabelDrawable.draw();
    m_goalDrawable.draw();
}

bool LevelStatus::moreMoves()
{
    return m_goal->movesLeft() > 0;
}

bool LevelStatus::objectiveMet()
{
    return m_goal->met();
}

int LevelStatus::score() const
{
    return m_score;
}

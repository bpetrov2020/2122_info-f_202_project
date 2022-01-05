#ifndef LEVEL_STATUS_H
#define LEVEL_STATUS_H

#include "level_goal.hpp"
#include "observer.hpp"
#include "common.hpp"

class LevelData;

class LevelStatus : public DrawableContainer, public Subject, public Observer
{
private:
    // TODO put all score additions in here
    int m_score {0};
    Text m_scoreLabelDrawable;
    Text m_scoreDrawable;

    // TODO init from data
    std::shared_ptr<LevelGoal> m_goal; // {std::make_shared<EventOccurGoal>(3, Event::IcingCleared, 3)};

    // TODO init from data
    Text m_movesLeftLabelDrawable;
    Text m_movesLeftDrawable;

    Text m_goalLabelDrawable;
    Text m_goalDrawable;

    int scoreValueOf(Event) const;

public:
    LevelStatus(const Point &center, int width, int height, LevelData &data) noexcept;

    void draw() override;

    void updateScore(int toAdd);
    void update(Event event) override;

    bool moreMoves();
    bool objectiveMet();

    int score() const;
};

#endif // LEVEL_STATUS_H

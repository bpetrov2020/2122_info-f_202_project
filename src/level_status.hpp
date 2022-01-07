#ifndef LEVEL_STATUS_H
#define LEVEL_STATUS_H

#include "level_goal.hpp"
#include "observer.hpp"
#include "common.hpp"

class LevelData;

/**
 * LevelStatus class,
 *
 * contains information about the being
 * played level, score and goal status
 */
class LevelStatus : public DrawableContainer, public Subject, public Observer
{
private:
    int m_score {0};
    Text m_scoreLabelDrawable;
    Text m_scoreDrawable;

    std::shared_ptr<LevelGoal> m_goal;

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

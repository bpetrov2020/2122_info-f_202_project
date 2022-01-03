#ifndef LEVEL_GOAL_H
#define LEVEL_GOAL_H

#include "observer.hpp"
#include <string>

/**
 * LevelGoal class,
 * will handle the amount of remaining
 * moves to play before the player loses.
 *
 * When no more moves are left to play, a NoMoreMoves event is called.
 */
class LevelGoal : public Subject, public Observer
{
public:
    void update(Event) override;

    int movesLeft() const;

    virtual bool met() const = 0;
    virtual std::string progressToString() = 0;

    LevelGoal(int moves) noexcept
        : m_movesLeft {moves}
    {
    }
    virtual ~LevelGoal() noexcept = default;

protected:
    int m_movesLeft;
};

/**
 * This class will handle the goal,
 * given through a parameter.
 *
 * Besides that, we also handle the moves left
 * to play, through the LevelGoal inheritance.
 */
class EventOccurGoal : public LevelGoal
{
public:
    EventOccurGoal(int moves, Event eventWaiting, int remaining) noexcept;

    void update(Event) override;
    bool met() const override;
    std::string progressToString() override;

protected:
    Event m_eventWaiting;
    int m_remaining;
};

#endif // LEVEL_GOAL_H

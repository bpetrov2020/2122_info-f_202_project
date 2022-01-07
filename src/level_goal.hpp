#ifndef LEVEL_GOAL_H
#define LEVEL_GOAL_H

#include "observer.hpp"

#include <string>

class LevelGoal : public Subject, public Observer
{
public:
    LevelGoal(int moves) noexcept : m_movesLeft {moves} { }
    virtual ~LevelGoal() noexcept = default;

    void update(Event) override;

    int movesLeft() const;

    virtual bool met() const = 0;
    virtual std::string progressToString() = 0;

protected:
    int m_movesLeft;
};

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

#include "level_goal.hpp"

#include "event.hpp"

#include <cassert>

void LevelGoal::update(Event event)
{
    if (event == Event::TurnEnd) {
        --m_movesLeft;
        assert(m_movesLeft >= 0);

        notifyObservers(Event::GoalChanged);

        if (met()) {
            notifyObservers(Event::GoalReached);
        } else if (m_movesLeft == 0) {
            notifyObservers(Event::NoMoreMoves);
        }
    }
}

int LevelGoal::movesLeft() const
{
    return m_movesLeft;
}

EventOccurGoal::EventOccurGoal(int moves, Event eventWaiting, int target) noexcept
    : LevelGoal {moves}
    , m_eventWaiting {eventWaiting}
    , m_remaining {target}
{
}

void EventOccurGoal::update(Event event)
{
    LevelGoal::update(event);

    if (event == m_eventWaiting) {
        --m_remaining;
        assert(m_remaining >= 0);

        notifyObservers(Event::GoalChanged);
    }
}

bool EventOccurGoal::met() const
{
    /*std::cout << m_remaining << std::endl; */
    return m_remaining == 0;
}

std::string EventOccurGoal::progressToString()
{
    return std::to_string(m_remaining);
}

/**
 * Classes implementing the Observer design pattern.
 */

#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include "event.hpp"

#include <algorithm>
#include <vector>

/**
 * Observer and subject class,
 *
 * Used to make objects communicate events with each others.
 */
class Observer
{
public:
    virtual void update(Event) = 0;

    virtual ~Observer() noexcept = default;
};

class Subject
{
public:
    virtual void registerObserver(Observer *observer);
    virtual void removeObserver(Observer *observer);
    virtual void notifyObservers(Event event) const;

    virtual ~Subject() noexcept = default;

protected:
    std::vector<Observer *> m_observers {};
};

#endif // OBSERVER_HPP

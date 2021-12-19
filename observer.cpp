#include "observer.hpp"

#include <algorithm>

void Subject::registerObserver(Observer *observer)
{
    m_observers.push_back(observer);
}

void Subject::removeObserver(Observer *observer)
{
    std::remove(std::begin(m_observers), std::end(m_observers), observer);
}

void Subject::notifyObservers(Event event) const
{
    for (auto &observer : m_observers)
        observer->update(event);
}

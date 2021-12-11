#ifndef EVENT_HPP
#define EVENT_HPP

enum class Event {
    CellCleared,
    TurnEnd,
    NeighbourMatched,
    NeighbourCleared,
    FallStateEnd,
    cellContentAnimationFinished,
    gridSelectionChanged
};

#endif

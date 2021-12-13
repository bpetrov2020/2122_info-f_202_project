#ifndef EVENT_HPP
#define EVENT_HPP

enum class Event {
    GoalReached
    , NoMoreMoves
    , GoalChanged
    , IcingCleared
    , CellCleared
    , TurnEnd
    , NeighbourMatched
    , NeighbourCleared
    , FallStateEnd
    , cellContentAnimationFinished
    , gridSelectionChanged
};

#endif

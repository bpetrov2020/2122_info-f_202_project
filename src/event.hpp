#ifndef EVENT_HPP
#define EVENT_HPP

/// enumerates all the different events that could happen
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
    , SelectionChanged
    , HintAnimationFinished
    , PulseAnimationFinished
    , LevelPassed
    , LevelNotPassed
};

#endif

#ifndef EVENT_HPP
#define EVENT_HPP

enum class Event {
    GoalReached
    , NoMoreMoves
    , NoMoreCombinations
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
    , LevelPassed
    , LevelNotPassed
    , ThreeMatch
    , StripedMatch
    , WrappedMatch
    , ColourBombMatch
};

#endif

#ifndef EVENT_HPP
#define EVENT_HPP

/// enumerates all the different events that could happen
enum class Event {
    GoalReached
    , NoMoreMoves
    , NoMoreCombinations
    , GoalChanged
    , IcingCleared
    , FruitCleared
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

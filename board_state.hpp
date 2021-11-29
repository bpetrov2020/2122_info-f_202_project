#ifndef BOARD_STATE_HPP
#define BOARD_STATE_HPP

#include "common.hpp"
#include "event.hpp"
#include "grid.hpp"

class Grid;

/**
 * Base class to any state
 *
 * A state is a particular moment in the execution
 * of a program. Depending on its nature, the outcome of
 * events will be different. Each state reacts differently
 * to events.
 *
 * Each state should have preconditions and postconditions
 * the grid must satisfy to be in the given state.
 *
 * @param grid the grid the state is tied to
 */
class State : public Interactive
{
    protected:
        Grid &grid;

        /**
         * Store any reference to cells whose
         * animation is finished. Empty by default.
         */
        std::vector<Point> waitingList {};
    public:
        State(Grid &grid)
            : grid{grid}
        { }

        // No interactions by default
        void mouseMove(Point mouseLoc) override { }
        void mouseClick(Point mouseLoc) override { }
        void mouseDrag(Point mouseLoc) override { }

        bool isWaiting() const;
        virtual void animationFinished(const Point &p) = 0;

        virtual void update(Event e) { };

        // TODO Place all events in one function
        /* void update(Event e); */
};

class EditState : public State
{
    private:
        void selectionChanged();
    public:
        EditState(Grid &grid) noexcept
            : State{grid}
        { }

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void update(Event e) override
        {
            switch (e) {
                case Event::gridSelectionChanged:
                    selectionChanged();
                    break;
            }
        }

        void animationFinished(const Point &p) override { }
};

/**
 * State that uses combinations
 *
 * Player should not be able to interact with the program
 * during this state.
 *
 * @param grid the grid the state is tied to
 */
class MatchState : public State
{
    public:
        MatchState(Grid &grid) noexcept
            : State{grid}
        { }

        bool isInCombination(const Point &point);
        std::vector<std::vector<Point>> combinationsFrom(const Point &p);
        bool processCombinationsFrom(const Point &p);
};

/**
 * Ready state
 *
 * The game is in a ReadyState whenever any animations
 * are done and it's waiting for the player to do an action.
 * This action is selecting cells to swap.
 *
 * Preconditions:
 *  - no animations are playing
 *
 * @param grid the grid the state is tied to
 */
class ReadyState : public MatchState
{
    private:
        /**
         * Should be called by the grid whenever the selection
         * is changed.
         */
        void selectionChanged();
    public:
        ReadyState(Grid &grid, bool initG = false) noexcept
            : MatchState{grid}
        {
            std::cout << "Entering Ready state" << std::endl;
            if (initG) {
                initGrid();
            }
        }

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void initGrid();
        void replaceGrid();

        void animationFinished(const Point &p) override;

        void update(Event e) override
        {
            switch (e) {
                case Event::gridSelectionChanged:
                    selectionChanged();
                    break;
            }
        }
};

/**
 * Fallstate, when cell contents fall
 *
 * This state must be used when there are empty cells
 * on the board. The falling process is done inside this
 * state.
 *
 * Preconditions:
 *  - at least one empty cell in the grid
 *
 * @param grid the grid the state is tied to
 */
class FallState : public MatchState
{
    public:
        FallState(Grid &grid) noexcept
            : MatchState{grid}
        {
            fillGrid();
            std::cout << "Entering Fall" << std::endl; 
        }

        bool fillGrid();
        bool makeFall(const Point &p);
        bool canFallTo(const Point &p, Direction target);
        bool isFillableByFall(const Point &point);

        void animationFinished(const Point &p) override;
};

/**
 * SwapState, when two cell contents are swapping
 *
 * The clearing animaitions should be done outside of
 * this state.
 *
 * Preconditions:
 *  - there must be two cell content swapping positions
 *  - so, two animations, no more, no less
 *
 * @param grid the grid the state is tied to
 */
class SwapState : public MatchState
{
    private:
        bool swapBack{false};
    public:
        SwapState(Grid &grid) noexcept
            : MatchState{grid}
        {
            std::cout << "Entering Swap" << std::endl;
        }

        void animationFinished(const Point &p) override;
};

/**
 * ClearState, when cell contents are clearing
 *
 * This state is to be used when there are cell contents
 * in the process of clearing (with an animation).
 *
 * Preconditions:
 *  - at least one clear animation
 *
 * @param grid the grid the state is tied to
 */
class ClearState : public State
{
    public:
        ClearState(Grid &grid) noexcept
            : State{grid}
        {
            std::cout << "Entering Clear" << std::endl;
        }

        void animationFinished(const Point &p) override;
};

#endif

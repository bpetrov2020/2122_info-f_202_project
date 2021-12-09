#ifndef BOARD_STATE_HPP
#define BOARD_STATE_HPP

#include "common.hpp"
#include "event.hpp"
#include "grid.hpp"

class Grid;
/* enum class Color { */
/*     Blue, */
/*     Green, */
/*     Cyan, */
/*     Magenta, */
/*     Red, */
/*     Yellow */
/* }; */

/* class CandyColor */
/* { */
/*     private: */
/*         static constexpr unsigned colorCount{ 6 }; */

/*         static constexpr std::array<std::string, colorCount> names { */
/*             "Blue", */
/*             "Green", */
/*             "Cyan", */
/*             "Magenta", */
/*             "Red", */
/*             "Yellow" */
/*         }; */

/*         static constexpr std::array<int, colorCount> normal { */
/*             0x22a0fd00,  // Blue */
/*             0x4ad81200,  // Green */
/*             0xfe810200,  // Orange */
/*             0xd31ded00,  // Purple */
/*             0xe3010200,  // Red */
/*             0xffff8a00   // Yellow */
/*         }; */

/*         unsigned colorIndex{ -1 }; */
/*     public: */
/*         Color(std::string name) */
/*         { */
/*             setFromName(name); */
/*         } */

/*         void setFromName(std::string name) */
/*         { */
/*             for (unsigned i=0; i<colorCount-1; ++i) { */
/*                 if (names[i] == name) { */
/*                     colorIndex = i; */
/*                     break; */
/*                 } */
/*             } */
/*         } */

/*         bool isValid() { return colorIndex >= 0 && colorIndex < colorCount; } */

/*         int getValue() const { return normal.at(colorIndex); }; */

/*         std::string getName(); */
/*         int getLightValue(); */
/*         int getDarkValue(); */
/* }; */

/* CandyColor color{ "Blue" }; */

/* color.getFlNormal(); */
/* color.getFlLight(); */
/* color.getFlDark(); */

/* color.getName(); */

class Combination
{
    private:
        Point origin;
        std::vector<Point> vertical {};
        std::vector<Point> horizontal {};
    public:
        Combination(const Point &point);

        auto getOrigin() const;

        void addVerticalElement(const Point &elem);
        void addHorizontalElement(const Point &elem);
        void addElement(const Point &elem, Direction direction);

        // These include the origin
        std::size_t getVerticalCount() const;
        std::size_t getHorizontalCount() const;
        std::size_t getTotalCount() const;

        // These don't include the origin
        auto getVerticalElements();
        auto getHorizontalElements();
        auto getAllElements();
};

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

        virtual void draw() { }

        // No interactions by default
        void mouseMove(Point) override { }
        void mouseClick(Point) override { }
        void mouseDrag(Point) override { }

        bool isWaiting() const;
        virtual void gridAnimationFinished(const Point &p) = 0;

        virtual void update(Event) { }

        virtual void notifyCells(Event e);

        // TODO Place all events in one function
        /* void update(Event e); */
};

class MessageState : public State, DrawableContainer
{
    protected:
        Text message;
        bool messageFinished{false};
    public:
        MessageState(Grid &grid, std::string msg, int duration = 60) noexcept;

        void draw() override;
        void gridAnimationFinished(const Point &) override { }
        void animationFinished(AnimationT animationType) override;
};

class NoActionState : public MessageState
{
    public:
        NoActionState(Grid &grid) noexcept;

        void draw() override;
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
                default:
                    break;
            }
        }

        void gridAnimationFinished(const Point &) override { }
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
        Combination getCombinationContaining(const Point &p, bool rec = true);
        bool processCombinationContaining(const Point &p);
};

class LevelData;

class GridInitState : public MatchState
{
    private:
        void putInitialContent(LevelData &data);
        void fillEmptyCells();
    public:
        GridInitState(Grid &grid, LevelData &data);

        void draw() override;
        void gridAnimationFinished(const Point &) override { }
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
        bool hasPossibleAction{true};
    public:
        ReadyState(Grid &grid, bool initG = false) noexcept;

        void draw() override;

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        // testing functions
        /*void twoColourBombs() {
            Point point{1, 1};
            grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Red);
            point = {1, 2};
            grid.put(point, ContentT::ColourBomb, StandardCandy::Color::Blue);
        }*/

        void replaceGrid();
        bool isActionPossible();

        void gridAnimationFinished(const Point &p) override;

        void update(Event e) override
        {
            switch (e) {
                case Event::gridSelectionChanged:
                    selectionChanged();
                    break;
                default:
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

        void gridAnimationFinished(const Point &p) override;
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

        void gridAnimationFinished(const Point &p) override;
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

        void gridAnimationFinished(const Point &p) override;
};

#endif

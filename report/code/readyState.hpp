class ReadyState : public MatchState
{
    private:
        /**
         * Should be called by the grid whenever the selection
         * is changed.
         */
        void selectionChanged();
        bool hasPossibleAction{true};

        Combination bestCombination{Point{0, 0}};

        static constexpr int hintInterval {120};
        int countToNextHint{hintInterval};
    public:
        ReadyState(View *level, Grid &grid, bool initG = false) noexcept;

        void draw() override;

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void showHint();
        void suspendHint();

        Combination getBestCombination();
        Combination getBestSpecialCombination();

        void replaceGrid();
        bool isActionPossible();

        void gridAnimationFinished(const Point &p) override;

        void update(Event e) override;
};

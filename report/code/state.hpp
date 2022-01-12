class State : public Interactive
{
protected:
    View *level;
    Grid &grid;

    /**
     * Store any reference to cells whose
     * animation is finished. Empty by default.
     */
    std::vector<Point> waitingList {};
public:
    State(View *level, Grid &grid);

    State(const State &) = delete;
    State operator=(const State &) = delete;

    virtual void draw() { }

    // No interactions by default
    void mouseMove(Point) override;
    void mouseClick(Point) override;
    void mouseDrag(Point) override;

    bool isWaiting() const;
    virtual void gridAnimationFinished(const Point &p) = 0;

    virtual void update(Event event);

    virtual void notifyCells(Event e);
};

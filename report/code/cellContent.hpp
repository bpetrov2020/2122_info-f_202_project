class CellContent : public DrawableContainer
{
protected:
    Grid &grid;
    Cell *containerCell;

    bool m_isPulsing {false};
    bool m_pulseFinished {false};
public:
    CellContent(Grid &grid, Cell *cell, std::shared_ptr<Shape> drawable);

    CellContent(const CellContent& c) = delete;
    CellContent operator=(const CellContent& c) = delete;

    void draw() override;

    void update(Event);

    void animationFinished(AnimationT) override;

    virtual ContentT getType() = 0;
};

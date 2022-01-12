class Fruit : public ClearableCellContent, public MovableCellContent
{
public:
    enum class Type {
        Cherry = 4,
        Hazelnut = 6
    };

protected:
    Type type;
    Fruit(Grid &grid, Cell *cell, const Point &center, std::shared_ptr<AnimatableShape> shape);
    bool isAtBottom();

public:
    Fruit(Grid &grid, Cell *cell, const Point &center, int side, Type type);

    void clear() override;
    void update(Event e) override;

    void draw() override;

    void animationFinished(AnimationT a) override;

    ContentT getType() override { return ContentT::Fruit; }
};

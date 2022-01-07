struct Interactive
{
    virtual ~Interactive() noexcept = default;

    virtual void mouseMove(Point) = 0;
    virtual void mouseClick(Point) = 0;
    virtual void mouseDrag(Point) = 0;
};

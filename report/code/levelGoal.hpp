class LevelGoal : public Subject, public Observer
{
public:
    LevelGoal(int moves) noexcept : m_movesLeft {moves} { }
    virtual ~LevelGoal() noexcept = default;

    void update(Event) override;

    int movesLeft() const;

    virtual bool met() const = 0;
    virtual std::string progressToString() = 0;

protected:
    int m_movesLeft;
};

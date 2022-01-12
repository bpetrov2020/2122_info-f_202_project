class EventOccurGoal : public LevelGoal
{
public:
    EventOccurGoal(int moves, Event eventWaiting, int remaining) noexcept;

    void update(Event) override;
    bool met() const override;
    std::string progressToString() override;

protected:
    Event m_eventWaiting;
    int m_remaining;
};

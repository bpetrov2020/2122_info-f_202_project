class LevelStatus : public DrawableContainer, public Subject, public Observer
{
private:
    int m_score {0};
    Text m_scoreLabelDrawable;
    Text m_scoreDrawable;

    std::shared_ptr<LevelGoal> m_goal;
    Text m_goalLabelDrawable;
    Text m_goalDrawable;

    Text m_movesLeftLabelDrawable;
    Text m_movesLeftDrawable;

    int scoreValueOf(Event) const;

public:
    LevelStatus(const Point &center, int width, int height, LevelData &data) noexcept;

    void draw() override;

    void updateScore(int toAdd);
    void update(Event event) override;

    // On the progession of the game
    bool moreMoves();
    bool objectiveMet();

    int score() const;
};

class Level : public View, public Subject
{
    private:
        LevelData m_data;
        LevelStatus m_status;
        Grid m_board;
        std::shared_ptr<State> m_boardController {nullptr};

        inline int gridSide(Fl_Window &win);
    public:
        Level(Fl_Window& window, Game& game, int levelNumber);

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void draw() override;

        void setState(std::shared_ptr<State> state);
        void replayLevel();
        void playNextLevel();

        void update(Event event) override;
        void updateScore(int toAdd);

        int getColorRange() const;
        StandardCandy::Color getRandomCandyColor() const;
};

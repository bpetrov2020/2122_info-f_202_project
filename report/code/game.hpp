class Game : public Interactive
{
private:
    Fl_Window& window;
    std::shared_ptr<View> view;

    int bestScore;
    void writeScore();

    int numberOfLevels{9};
public:
    Game(Fl_Window& win);

    void mouseMove(Point) override;
    void mouseClick(Point) override;
    void mouseDrag(Point) override;

    void draw();

    void loadView(std::shared_ptr<View>);

    void updateScore(int);
    void resetScore();
    int getBestScore() const;

    int getNumberOfLevels() const;
};

class SplashScreen : public View
{
private:
    DrawableContainer author;
    bool toBeReplaced {false}; // Whether or not the next screen should be loaded
public:
    SplashScreen(Fl_Window &window, Game &game, std::string author, int fontSize, int duration);

    // Mouse interactions and updates are disabled
    void mouseMove(Point) override { }
    void mouseClick(Point) override { }
    void mouseDrag(Point) override { }
    void update(Event) override { }

    void draw() override;

    void animationFinished(AnimationT a) override;
};

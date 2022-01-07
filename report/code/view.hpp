class View : public DrawableContainer, public Interactive, public Observer
{
protected:
    Fl_Window &window;
    Game &game;

public:
    View(Fl_Window &window, Game &game);
};

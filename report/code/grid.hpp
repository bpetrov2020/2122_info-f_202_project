class Grid : public DrawableContainer, public Interactive
{
protected:
    std::vector<std::vector<std::shared_ptr<Cell>>> matrix {};
    int selectedCount = 0;

    // Dimentions
    int cellContentSide = 10;
    int cellSide = 15;
    int colSize;
    int rowSize;

    std::shared_ptr<State> state;
public:
    Grid(Point center, int width, int height, LevelData &data, Fl_Color color = FL_BLACK);
    Grid(Point center, int width, int height, int rows, int columns, Fl_Color color = FL_BLACK);

    class Iterator;
    Iterator begin();
    Iterator end();

    // Getters for cells
    Cell &at(const Point &p);
    Cell &at(const Point &c, Direction d);

    void select(const Point &c);
    int getSelectedCount() const;
    std::vector<Point> getSelected();
    void clearSelection();

    // Here diagonal neighbours are not considered as neighbours
    bool areNeighbours(const Point& c1, const Point& c2);
    std::vector<Point> neighboursOf(const Point& c);

    void draw() override;

    void setState(std::shared_ptr<State> newState);

    // Mouse interactions
    void mouseMove(Point mouseLoc) override;
    void mouseClick(Point mouseLoc) override;
    void mouseDrag(Point mouseLoc) override;

    unsigned colCount() const;
    unsigned rowCount() const;

    bool isIndexValid(const Point &p, Direction d) const;
    bool isIndexValid(const Point &p) const;

    void update(Event e);
    void cellContentAnimationFinished(const Point &p);

    int getRowSize() const;
    int getCellContentSide() const;

    bool hint(Point p);
    void removeAnimations();
};

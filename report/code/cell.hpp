class Cell : public DrawableContainer, public Interactive
{
    protected:
        Grid &grid;
        Point index;  // position in the grid
        std::shared_ptr<CellContent> content;
        bool selected = false;
        bool processedThisClearState{ false };
        bool lastSelected = false;

    public:
        Cell(Point center, int width, int height, Point index, Grid &grid);

        // Two functions are needed to have the background
        // and foreground drawn separately
        void draw() override;
        void drawContent();

        void mouseMove(Point mouseLoc) override;
        void mouseClick(Point mouseLoc) override;
        void mouseDrag(Point mouseLoc) override;

        void update(Event e);

        bool clear();
        void clearWithoutAnimation();
        void removeContent();

        bool isEmpty() const;

        bool moveContentTo(Cell &other);
        bool swapContentWith(const Point &p);
        void contentWasSwappedWith(const Point &p);
        bool swapContentWithWithoutAnimation(const Point &p);

        auto &getContent();
        void setContent(std::shared_ptr<CellContent> c);

        // actions on lastSelected
        bool isLastSelected();
        void setLastSelected(bool state);

        bool isSelected() const;

        const Point getIndex() const;

        bool toggleSelected();

        bool wasProcessedThisClear() const;

        bool hint();
        void removeContentAnimation();
};

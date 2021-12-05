#include "game.hpp"

/*----------------------------------------------------------
 * View
 *--------------------------------------------------------*/

View::View(Fl_Window& win, Game& g)
    : DrawableContainer{
        std::make_shared<Rectangle>(
            Point{win.w()/2, win.h()/2},
                win.w(),
                win.h()
        )
    },
    window{win},
    game{g} { }

/*----------------------------------------------------------
 * Game
 *--------------------------------------------------------*/

Game::Game(Fl_Window& win)
    : window{win},
    /* view{std::make_shared<SplashScreen>(win, *this, "Authors", 15, 120)} { } TODO */
    view{std::make_shared<Level>(win, *this, "level1.txt")}
{ }


void Game::mouseMove(Point mouseLoc)  { if(view) view->mouseMove(mouseLoc); }
void Game::mouseClick(Point mouseLoc) { if(view) view->mouseClick(mouseLoc); }
void Game::mouseDrag(Point mouseLoc)  { if(view) view->mouseDrag(mouseLoc); }

void Game::draw() { if(view) view->draw(); }

void Game::loadView(std::shared_ptr<View> v)
{
    view.reset();
    view = std::move(v);
}

/*----------------------------------------------------------
 * SplashScreen
 *--------------------------------------------------------*/

SplashScreen::SplashScreen(
        Fl_Window& win,
        Game& game,
        std::string authors,
        int fontSize,
        int duration
        )
    : View{win, game},
    author{std::make_shared<Text>(Point{win.w()/2, win.h()/2}, authors, fontSize)}
{
    addAnimation(std::make_shared<StillAnimation>(duration));
}

void SplashScreen::draw()
{
    DrawableContainer::draw();  // draw the background
    author.draw();              // draw the author's name
    if (toBeReplaced)
        game.loadView(std::make_shared<Level>(window, game, "level1.txt"));
}

void SplashScreen::animationFinished(AnimationT animationType)
{
    switch (animationType) {
        case AnimationT::StillAnimation:
            toBeReplaced = true;
            break;
        case AnimationT::ScaleAnimation:
        case AnimationT::MoveAnimation:
            break;
    }
}

/*----------------------------------------------------------
 * LevelData
 *--------------------------------------------------------*/

LevelData::LevelData(std::string filename)
{
    extractDataFrom(filename);
}

void LevelData::extractDataFrom(std::string filename)
{
    std::ifstream file{ filename };
    if (!file)
        throw std::runtime_error{"LevelData: Invalid filename given " + filename};

    std::string line;
    while (std::getline(file, line)) {
        processLine(line);
    }
}

void LevelData::processLine(std::string line)
{
    std::istringstream is{ line };
    std::string category;
    is >> category;
    if (!is)
        throw std::runtime_error{"LevelData: First element should be a string"};

    if (category == "Size") {
        is >> gridSize;
        if (gridSize<3 || gridSize>26)
            throw std::runtime_error{"LevelData: Wrong size given"};

    } else if (category == "Wall") {
        fillFrom(wallsPos, is);

    } else if (category == "SingleIcing") {
        fillFrom(singleIcingPos, is);

    } else if (category == "DoubleIcing") {
        fillFrom(doubleIcingPos, is);

    } else {
        throw std::runtime_error{"LevelData: Unknow category used"};
    }
}

void LevelData::fillFrom(std::vector<Point> &vect, std::istream &is)
{
    if (!vect.empty())
        throw std::runtime_error{"LevelData: There should not be multiple sections for the same content"};
    if (gridSize == -1)
        throw std::runtime_error{"LevelData: Grid size should be initialized before contents"};

    for (Point p; is>>p; ) {
        if (p.x>=0 && p.x<gridSize && p.y>=0 && p.y<gridSize)
            vect.push_back(p);
        else
            throw std::runtime_error{"LevelData: Out of range point"};
    }

    if (!is.eof())
        throw std::runtime_error("Illegal input");
}

/*----------------------------------------------------------
 * Level
 *--------------------------------------------------------*/

Level::Level(Fl_Window& window, Game& game, const std::string &filename)
    : View{window, game},
    board{nullptr}
{
    initFromFile(filename);
}

void Level::initFromFile(std::string filename)
{
    LevelData data{ filename };
    board = std::make_shared<Grid>(Point{window.w()/2, window.h()/2}, window.w()-50, window.h()-50, data);
    /* status = std::make_shared<LevelStatus>(data.getLevelData())); */
}

void Level::draw()
{
    DrawableContainer::draw();  // background of the level
    board->draw();
}

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
    :
        m_levelName{filename}
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
        if (!is || gridSize<3 || gridSize>26)
            throw std::runtime_error{"LevelData: Wrong size given"};

    } else if (category == "ColorRange") {
        is >> colorRange;
        if (!is || colorRange < 2 || colorRange > 6)
            throw std::runtime_error{"LevelData: Wrong colorRange given"};

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
 * LevelStatus
 *--------------------------------------------------------*/

LevelStatus::LevelStatus(const Point &center, int width, int height) noexcept
    :
        DrawableContainer{std::make_shared<Rectangle>(center, width, height)},
        scoreLabelDrawable{Point{width/4, center.y-height/2+height/3}, "Score", height/6, FL_BLACK},
        scoreDrawable{Point{width/4, center.y-height/2+height/3*2}, "0", height/5, FL_BLACK},
        movesLeftLabelDrawable{Point{width/2, center.y-height/2+height/3}, "Moves", height/6, FL_BLACK},
        movesLeftDrawable{Point{width/2, center.y-height/2+height/3*2}, "3", height/5, FL_BLACK}
{ }

void LevelStatus::updateScore(int toAdd)
{
    score += toAdd;
    scoreDrawable.setString(std::to_string(score));
}

void LevelStatus::update(Event event)
{
    switch (event) {
        case Event::TurnEnd:
            --movesLeft;
            movesLeftDrawable.setString(std::to_string(movesLeft));
            break;
        default:
            break;
    }
}

void LevelStatus::draw()
{
    DrawableContainer::draw();
    scoreLabelDrawable.draw();
    scoreDrawable.draw();
    movesLeftLabelDrawable.draw();
    movesLeftDrawable.draw();
}

bool LevelStatus::moreMoves()
{
    return movesLeft > 0;
}

// TODO implement this
bool LevelStatus::objectiveMet()
{
    return false;
}

/*----------------------------------------------------------
 * Level
 *--------------------------------------------------------*/

// TODO make adaptable to height
Level::Level(Fl_Window& window, Game& game, const std::string &filename)
    : View{window, game},
    m_data{filename},
    m_status{Point{window.w()/2, window.h()/12*11}, gridSide(window), gridSide(window)/5},
    m_board{Point{window.w()/2, window.h()/12*5}, gridSide(window), gridSide(window), m_data},
    m_boardController{nullptr}
{
    setState(std::make_shared<GridInitState>(*this, m_board, m_data));
}

inline int Level::gridSide(Fl_Window &win)
{
    return win.h() >= win.w() ? win.w() : win.h()/6*5;
}

void Level::draw()
{
    DrawableContainer::draw();  // background of the level
    m_board.draw();
    m_status.draw();
    m_boardController->draw();
}

void Level::setState(std::shared_ptr<State> state)
{
    m_boardController = state;
    m_board.setState(state);
}

void Level::replayLevel()
{
    game.loadView(std::make_shared<Level>(window, game, m_data.levelName()));
}

// TODO: implement it
void Level::playNextLevel()
{
}

void Level::update(Event event)
{
    m_status.update(event);
    switch (event) {
        case Event::TurnEnd:
            if (m_status.objectiveMet())
                setState(std::make_shared<LevelPassedState>(*this, m_board));
            if (!m_status.moreMoves())
                setState(std::make_shared<LevelNotPassedState>(*this, m_board));
            break;
        default:
            break;
    }
}

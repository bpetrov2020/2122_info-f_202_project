#include "level_data.hpp"

#include "point.hpp"
#include "event.hpp"
#include "level_goal.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

LevelData::LevelData(int levelNumber)
    : m_levelNumber{levelNumber}
{
    extractDataFrom("data/levels/" + std::to_string(levelNumber)+".txt");
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

    if (m_goalType == "")
        throw std::runtime_error{"Goal cannot be empty"};  // for now

    if (m_goalType == "Icing") {
        auto icingCount {2*getDoubleIcingPos().size() + getSingleIcingPos().size()};
        m_goal = std::make_shared<EventOccurGoal>(m_movesToGoal, Event::IcingCleared, icingCount);

    } else if (m_goalType == "Fruit") {
        auto fruitCount {getCherryPos().size() + getHazelnutPos().size()};
        m_goal = std::make_shared<EventOccurGoal>(m_movesToGoal, Event::FruitCleared, fruitCount);
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
        is >> m_gridSize;
        if (!is || m_gridSize<3 || m_gridSize>26)
            throw std::runtime_error{"LevelData: Wrong size given"};

    } else if (category == "ColorRange") {
        is >> m_colorRange;
        if (!is || m_colorRange < 2 || m_colorRange > 6)
            throw std::runtime_error{"LevelData: Wrong colorRange given"};

    } else if (category == "Goal") {
        is >> m_goalType;
        if (!is || (m_goalType != "Icing" && m_goalType != "Fruit"))
            throw std::runtime_error{"LevelData: Wrong goal argument"};

        is >> m_movesToGoal;
        if (!is)
            throw std::runtime_error{"LevelData: Moves required in goal"};

    } else if (category == "Wall") {
        fillFrom(m_wallsPos, is);

    } else if (category == "SingleIcing") {
        fillFrom(m_singleIcingPos, is);

    } else if (category == "DoubleIcing") {
        fillFrom(m_doubleIcingPos, is);

    } else if (category == "Cherry") {
        fillFrom(m_cherryPos, is);

    } else if (category == "Hazelnut") {
        fillFrom(m_hazelnutPos, is);

    } else {
        throw std::runtime_error{"LevelData: Unknow category used"};
    }
}

void LevelData::fillFrom(std::vector<Point> &vect, std::istream &is)
{
    if (!vect.empty())
        throw std::runtime_error{"LevelData: There should not be multiple sections for the same content"};
    if (m_gridSize == -1)
        throw std::runtime_error{"LevelData: Grid size should be initialized before contents"};

    for (Point p; is>>p; ) {
        if (p.x>=0 && p.x<m_gridSize && p.y>=0 && p.y<m_gridSize)
            vect.push_back(p);
        else
            throw std::runtime_error{"LevelData: Out of range point"};
    }

    if (!is.eof())
        throw std::runtime_error("Illegal input");
}

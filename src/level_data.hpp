#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <vector>
#include <string>
#include <memory>

class Point;
class LevelGoal;

/**
 * Container for the data needed to initialize a level
 *
 * The data should be of the following format:
 *
 * <Category> <value(s)>
 *
 * The categories available are:
 *  -  Size           "1"     int
 * [-] Wall           "1..*"  Point
 * [-] SingleIcing    "1..*"  Point
 * [-] DoubleIcing    "1..*"  Point
 *
 * Only the size is obligatory. And it should alway be the first one.
 */
class LevelData
{
    private:
        int m_levelNumber;

        int m_gridSize {-1};
        int m_colorRange {6};

        std::string m_goalType {};
        int m_movesToGoal {-1};
        std::shared_ptr<LevelGoal> m_goal{};

        std::vector<Point> m_wallsPos{};
        std::vector<Point> m_singleIcingPos{};
        std::vector<Point> m_doubleIcingPos{};
        std::vector<Point> m_hazelnutPos{};
        std::vector<Point> m_cherryPos{};

        // Functions used to fill the data from a file
        void extractDataFrom(std::string filename);
        void processLine(std::string line);
        void fillFrom(std::vector<Point> &vect, std::istream &is);
    public:
        LevelData(int levelNumber);

        int levelNumber() const { return m_levelNumber; }

        int movesToGoal() const { return m_movesToGoal; }
        auto goal() const { return m_goal; }
        auto goalTypeString() const { return m_goalType; }

        int getGridSize() const { return m_gridSize; }
        int getColorRange() const { return m_colorRange; }

        const auto &getWallsPos() const { return m_wallsPos; }
        const auto &getSingleIcingPos() const { return m_singleIcingPos; }
        const auto &getDoubleIcingPos() const { return m_doubleIcingPos; }
        const auto &getCherryPos() const { return m_cherryPos; }
        const auto &getHazelnutPos() const { return m_hazelnutPos; }
};

#endif // LEVEL_DATA_H

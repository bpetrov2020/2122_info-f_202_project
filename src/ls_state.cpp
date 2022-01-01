#include "ls_state.hpp"

#include "board_state.hpp"
#include "event.hpp"
#include "ls_view.hpp"

LevelSelectorState::LevelSelectorState(View *levelSelector, Grid &grid)
    : State {levelSelector, grid}
    , m_grid {grid}
{
    for (auto y{0}; y < m_grid.rowCount(); ++y)
        for (auto x{0}; x < m_grid.colCount(); ++x)
            m_grid.put(Point {x, y}, std::to_string(y*m_grid.colCount() + x+1));
}

void LevelSelectorState::draw()
{
    m_grid.draw();
    if (m_levelToLoad != -1) {
        std::cout << m_levelToLoad << std::endl;
        dynamic_cast<LevelSelector *>(level)->loadLevel(m_levelToLoad);
    }
}

void LevelSelectorState::mouseMove(Point mouseLoc)
{
    m_grid.mouseMove(mouseLoc);
}

void LevelSelectorState::mouseClick(Point mouseLoc)
{
    m_grid.mouseClick(mouseLoc);
}

void LevelSelectorState::mouseDrag(Point mouseLoc)
{
    m_grid.mouseDrag(mouseLoc);
}

void LevelSelectorState::update(Event event)
{
    if (event == Event::SelectionChanged)
        selectionChanged();
}

void LevelSelectorState::selectionChanged()
{
    auto selection = m_grid.getSelected();
    assert(selection.size() == 1);
    auto selected = selection.at(0);

    auto levelNo {std::dynamic_pointer_cast<TextContent>(m_grid.at(selected).getContent())->getString()};
    /* auto levelNo {"1"}; */
    m_levelToLoad = std::stoi(levelNo);

    /* auto levelNo {std::dynamic_pointer_cast<TextContent>(m_grid.at(selected).getContent()).getString()}; */
    /* dynamic_cast<LevelSelector *>(view).loadLevel(std::stoi(levelNo)); */

    /* dynamic_cast<LevelSelector *>(level)->loadLevel(1); */
}

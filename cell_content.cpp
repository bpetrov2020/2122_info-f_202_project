#include "cell_content.hpp"

/*----------------------------------------------------------
 * StandardCandy
 *--------------------------------------------------------*/

StandardCandy::StandardCandy(Point center, int side)
    :
        StandardCandy(center, side, static_cast<StandardCandy::Color>(std::rand()%6))
{ }

StandardCandy::StandardCandy(Point center, int side, StandardCandy::Color c)
    :
        /* MatchableCellContent(true), */
        CellContent(true, true),
        drawableContent{center, side, side, flRelative[static_cast<int>(c)]},
        color{c}
{ }

void StandardCandy::draw()
{
    drawableContent.draw();
}

bool StandardCandy::operator==(CellContent &other) const
{
    try {
        StandardCandy& otherCandy { dynamic_cast<StandardCandy&>(other) };
        return getColor() == otherCandy.getColor();
    } catch (const std::bad_cast& err) {
        throw std::runtime_error("StandardCandy::operator== -> provided argument could not be converted to a StandardCandy");
    }
}

/* bool StandardCandy::hasMatch(const StandardCandy &other) const */
/* { */
/*     return getColor() == other.getColor(); */
/* } */

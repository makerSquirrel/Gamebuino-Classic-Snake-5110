#include "coordinate.h"
#include <Gamebuino.h>

Coordinate Coordinate::g_nomPos = Coordinate();
Coordinate Coordinate::g_growNomPos = Coordinate(); // only for new mode
Coordinate Coordinate::g_shrinkNomPos = Coordinate(); // only for new mode
Coordinate Coordinate::g_wallNomPos = Coordinate(); // only for new mode,
Coordinate Coordinate::g_snakePos = Coordinate(); // contains head position of the snake, used for convenience only


/// returns true if actually moved
bool Coordinate::move(const int8_t& direction, bool isClassicMode, bool isOutOfBounds, bool isInArena)
{
    if (direction == PAUSE)
        return false;
    if (direction == UP)
        { m_y -= 1; }
    else if (direction == RIGHT)
        { m_x += 1; }
    else if (direction == DOWN)
        { m_y += 1; }
    else if (direction == LEFT)
        { m_x -= 1; }
    else
        { return false; }
    if (!isClassicMode && !isInArena && !isOutOfBounds) // moving through loop in wall.
    {
        if (direction == RIGHT && m_x > c_rasterX-1)
            m_x = -1;
        else if (direction == LEFT && m_x < 1)
            m_x = c_rasterX;
        else if (direction == UP && m_y < 1)
            m_y = c_rasterY;
        else if (direction == DOWN && m_y > c_rasterY-1)
            m_y = -1;
    }
    return true;
}

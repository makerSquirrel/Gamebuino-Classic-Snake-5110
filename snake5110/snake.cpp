#include "snake.h"
#include "coordinate.h"

Coordinate Snake::getRandomFreePos() const
{
    Coordinate newCoord;
    bool inUse = true;
    while(inUse)
    {
        newCoord = Coordinate(random(0, c_rasterX), random(0, c_rasterY));
        inUse = isPartOfSnake(newCoord, true);
        if (!inUse && Coordinate::g_nomPos.isInArena())
            inUse = ((newCoord == Coordinate::g_nomPos) ? true : false);
        if (!inUse && Coordinate::g_growNomPos.isInArena())
            inUse = ((newCoord == Coordinate::g_growNomPos) ? true : false);
        if (!inUse && Coordinate::g_shrinkNomPos.isInArena())
            inUse = ((newCoord == Coordinate::g_shrinkNomPos) ? true : false);
        if (!inUse && Coordinate::g_wallNomPos.isInArena())
            inUse = ((newCoord == Coordinate::g_wallNomPos) ? true : false);
    }
    return newCoord;
}

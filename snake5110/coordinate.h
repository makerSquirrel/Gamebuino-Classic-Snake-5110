#pragma once
#include <Arduino.h>
#include "globalVariables.h"

class Coordinate
{
public:

static Coordinate g_nomPos;
static Coordinate g_growNomPos; // only for new mode
static Coordinate g_shrinkNomPos; // only for new mode
static Coordinate g_wallNomPos; // only for new mode, TODO: need to store open Walls! only one xpos-hole-array and a yPos hole array needed!
static Coordinate g_snakePos; // contains head position of the snake, used for convenience only

    Coordinate() : m_x(0), m_y(0) {}
    // positions are in raster resolution, not pixel resolution
    /// isInRaster means the snake raster 20x11!
    Coordinate(int8_t x, int8_t y, bool isInRaster = true) :
        m_x(x), m_y(y)
    {
        if (isInRaster)
            return;
        /// TODO: implement case !isInRaster, maybe relevant for porting to Gamebuino Meta
    }
    
    bool operator==(const Coordinate& other) const
        { return m_x == other.m_x and m_y == other.m_y; }
    bool operator!=(const Coordinate& other) const
        { return !(*this == other); }
        
    inline int8_t x() const { return m_x; }
    /// converts raster value to pixel value for drawing
    inline int8_t xPixel() const { return m_x*4+2; }
    inline int8_t y() const { return m_y; }
    inline int8_t yPixel() const { return m_y*4+2; }
    
    bool isInArena() const
    {
        if (m_x > -1 && m_x < c_rasterX && m_y > -1 && m_y < c_rasterY)
           return true;
        return false;      
    }

    
    void setOffBounds() { m_x = -2; m_y -2; }
    
    /// returns true if actually moved
    bool move(const int8_t& direction, bool isClassicMode, bool isOutOfBounds, bool isInArena);
private:
    int8_t m_x;
    int8_t m_y;
};

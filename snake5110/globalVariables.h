#pragma once
// #ifndef GLOBALVARIABLES_H
// #define GLOBALVARIABLES_H
#include <Arduino.h>
#include <Gamebuino.h>
// #include<avr/pgmspace.h>

static Gamebuino gb;
/** ******************   GLOBAL CONSTANTS: */
static const bool c_debug = false;
static const int8_t c_rasterX = 20;
static const int8_t c_rasterY = 11;
static const int8_t c_maxLevel = 9;
static const uint8_t g_arenaSize = (c_rasterX)*(c_rasterY);
#define HIGHSCORE_COUNT 5 /// Rodot style Highscore
#define NAME_LETTERS 10 /// Rodot style Highscore

#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3
#define PAUSE -1

/** ******************   GLOBAL VARIABLES: */
volatile static bool g_nom;
volatile static bool g_isClassicMode; // always set when game starts
volatile static int8_t g_growNom; // not a bool, since eating a growNom will increase the snake length by 3
volatile static int16_t g_score;
volatile static int16_t g_cpuLoad; // for debugging
volatile static int16_t g_ramUsage; // for debugging
volatile static int8_t g_gameLevel; // max level 9
volatile static int8_t g_delayCounter;
volatile static int8_t g_lastButtonPressed;
volatile static int8_t g_levelModuloHelper; // small helper to store the modulo value to handle game level
extern const uint8_t font5x7[];
extern const uint8_t font3x5[];
// only needed for non-classic mode:
volatile static bool g_xWallsRemoved[c_rasterX+1];
volatile static bool g_yWallsRemoved[c_rasterY+1];

// #endif

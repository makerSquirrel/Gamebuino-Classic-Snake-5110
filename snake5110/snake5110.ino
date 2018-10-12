#include <SPI.h>
#include <Gamebuino.h>
#include <EEPROM.h>

#include "globalVariables.h"
#include "coordinate.h"
#include "snake.h"

Snake g_snake;

/// CC-BY-SA 2018 by Lady Awesome and MakerSquirrel, original Highscore code by Rodot (here a modified version in use)

/** ******************   GLOBAL SHIT: */
/// new mode
static int16_t g_highscoreScores[HIGHSCORE_COUNT];
static char g_highscoreNames[HIGHSCORE_COUNT][NAME_LETTERS + 1];
static int16_t g_minHighscore;
static uint8_t g_xWallsRemaining;
static uint8_t g_yWallsRemaining;
/// classic mode
static int16_t g_highScoresClassic[HIGHSCORE_COUNT];
static char g_highScoreNamesClassic[HIGHSCORE_COUNT][NAME_LETTERS + 1];
static int16_t g_minHighScoreClassic;

const char g_startGame[] PROGMEM = "Start Game";
const char g_startClassic[] PROGMEM = "Start Classic";
const char g_showHighscore[] PROGMEM = "Highscore"; //"Show Highscore";
const char g_setDifficulty[] PROGMEM = "Difficulty"; //"Set Game difficulty";
const char g_goToTitleScreen[] PROGMEM = "Titlescreen"; //"Back to Titlescreen";
const char g_goCredits[] PROGMEM = "Credits";

#define MAINMENU_LENGTH 6
const char* const g_mainMenu[MAINMENU_LENGTH] PROGMEM = {
  g_startGame,
  g_startClassic,
  g_showHighscore,
  g_setDifficulty,
  g_goToTitleScreen,
  g_goCredits
};

/// logo coded in PROGMEM puffer:
const uint8_t Snake5110Logo[] PROGMEM = {64,36,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0xFE,0x47,0xF2,0xF,0xE1,0xC0,0x8F,0xE4,
0xFE,0xA7,0xF5,0xF,0xE1,0xC1,0x4F,0xEA,
0xFE,0x47,0xF2,0xF,0xE1,0xC0,0x8F,0xE4,
0xE0,0x7,0x70,0x3E,0xF9,0xC0,0xE,0x0,
0xE0,0x7,0x77,0x3E,0xF9,0xDF,0xCE,0x0,
0xE0,0x7,0x77,0x3E,0xF9,0xDF,0xCE,0x0,
0xE0,0x7,0x77,0x38,0x39,0xDF,0xCE,0x0,
0xE0,0x7,0x77,0x3F,0xB9,0xFC,0xF,0xFE,
0xE0,0x7,0x77,0x3F,0xB9,0xFC,0xF,0xFE,
0xFF,0xE7,0x77,0x3F,0xB9,0xFC,0xF,0xFE,
0xFF,0xE7,0x77,0x3,0xB8,0x0,0x0,0xE,
0xFF,0xE7,0x77,0x3F,0xB9,0xFC,0xF,0xFE,
0x0,0xE7,0x77,0x3F,0xB9,0xFC,0xF,0xFE,
0x0,0xE7,0x77,0x3F,0xB9,0xFC,0xF,0xFE,
0x0,0xE7,0x77,0x38,0x39,0xDF,0xCE,0x0,
0x0,0xE7,0x77,0x38,0x39,0xDF,0xCE,0x0,
0x0,0xE7,0x77,0x38,0x39,0xDF,0xCE,0x0,
0x0,0xE7,0x77,0x0,0x38,0x1,0xCE,0x0,
0xFF,0xE7,0x7F,0x10,0x38,0x81,0xCF,0xFE,
0xFF,0xE7,0x7F,0x28,0x39,0x41,0xCF,0xFE,
0xFF,0xE7,0x7F,0x10,0x38,0x81,0xCF,0xFE,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
0x0,0x0,0x0,0x0,0x0,0x0,0xF2,0x4E,
0x0,0x0,0x0,0x0,0x0,0x0,0x86,0xD1,
0x0,0x0,0x0,0x0,0x0,0x0,0xE2,0x51,
0x0,0x0,0x0,0x0,0x0,0x0,0x12,0x51,
0x0,0x0,0x0,0x0,0x0,0x0,0x12,0x51,
0x0,0x0,0x0,0x0,0x0,0x0,0xE2,0x4E,
};

/** ******************   FUNCTION DEFINITIONS: */


bool isXWallRemoved(int8_t val)
{
    if (val < 0 || val > c_rasterX)
        return false;
    return g_xWallsRemoved[val];
}

bool isYWallRemoved(int8_t val)
{
    if (val < 0 || val > c_rasterY)
        return false;
    return g_yWallsRemoved[val];
}

/// check if out of bounds (not in arena and )
bool isOutOfBounds(const Coordinate& pos) 
{
    if (pos.isInArena())
        return false;
    if (g_isClassicMode)
        return true;
    if (pos.x() < -1 || pos.x() > c_rasterX)
        return true;
    if (pos.y() < -1 || pos.y() > c_rasterY)
        return true;
    if (isXWallRemoved(pos.x()) || isYWallRemoved(pos.y()))
        return false;
    return true;
}


void deleteRandomWallElement()
{
  if (g_xWallsRemaining == 0 && g_yWallsRemaining == 0)
    return;
  if (g_xWallsRemaining == 0 && g_yWallsRemaining == 1)
  {
    /// remove last y wall
    for(auto i = 0; i < c_rasterY+1; i++)
    {
      if (!g_yWallsRemoved[i])
        continue;
      g_yWallsRemoved[i] = true;
      g_yWallsRemaining = 0;
      return;
    }
  }
  if (g_xWallsRemaining == 1 && g_yWallsRemaining == 0)
  {
    /// remove last x wall
    for(auto i = 0; i < c_rasterX+1; i++)
    {
      if (!g_xWallsRemoved[i])
        continue;
      g_xWallsRemoved[i] = true;
      g_xWallsRemaining = 0;
      return;
    }
  }
  bool inUse = true;
  while(inUse)
  {
    uint8_t rngXElement = random(0, c_rasterX+1);
    uint8_t rngYElement = random(0, c_rasterY+1);
    bool removedY = g_yWallsRemoved[rngYElement];
    bool removedX = g_xWallsRemoved[rngXElement];
    if (!removedY)
    {
      inUse = false;
      g_yWallsRemoved[rngYElement] = true;
      g_yWallsRemaining--;
    }
    else if (!removedX)
    {
      inUse = false;
      g_xWallsRemoved[rngXElement] = true;
      g_xWallsRemaining--;
    }
  }
}


/// setup function
void setup()
{
  gb.begin();
  g_gameLevel = 1;
  gb.battery.show = false; //hide the battery indicator
  showTitleScreen();
  loadHighscores(false);
  loadHighscores(true);
  g_cpuLoad = 0;
  g_ramUsage = 0;
}


void showTitleScreen()
{
    gb.display.setFont(font5x7);
    gb.titleScreen(F(""), Snake5110Logo);
}


/// loop function, mostly used for menu only
void loop()
{
    if(!gb.update())
        return;
    gb.display.setFont(font5x7);
    int menuValue = gb.menu(g_mainMenu,MAINMENU_LENGTH);
    
    if (menuValue == 0) // Start Game
    {
        initGame(false);
        gameLoop();
    }
    else if (menuValue == 1) // Use the classic start
    {
        initGame(true);
        gameLoop();
    }
    else if (menuValue == 2) // Show Highscore
    {
        showHighscore(true);
        showHighscore(false);
    }
    else if (menuValue == 3) // Set Game difficulty
        { setDifficulty(); }
    else if (menuValue == 4) // Back to Titlescreen
        { showTitleScreen(); }
    else if (menuValue == 5) // Show the credits
        { showCredits(); }
    else
        { showTitleScreen(); }
}


/// game loop function
void gameLoop()
{
  int8_t lastTimeButtonPressed = PAUSE; /// explicitly stores the button pressed last time step
    while(true)
    {
        if(!gb.update())
            { continue; }
            
        if(gb.buttons.pressed(BTN_C))
            { return; }
            
        if (c_debug)
        {
          gb.display.cursorX = 3;
          gb.display.cursorY = 10;
          gb.display.setColor(BLACK, WHITE);
          gb.display.println(Coordinate::g_snakePos.x());
          gb.display.cursorX = 3;
          gb.display.println(Coordinate::g_snakePos.y());
        //   gb.display.cursorX = 3;
        //   gb.display.println(g_snake.size());
//         if (wallCollision() || snakeBite() || postGameCnt != 0)
//             postGameCnt++;
        }
        
        if(wallCollision())
            { gameOver(true); return; }
        if(snakeBite())
            { gameOver(false); return; }
            
        g_delayCounter++;
        bool timeChanged = false;

        if ((g_delayCounter % g_levelModuloHelper) == 0)
            timeChanged = true;
        drawArena();

        /// for new mode: ignore button presses where snake would turn backwards, old mode: die at that point
        if (gb.buttons.repeat(BTN_UP, 1))
        { g_lastButtonPressed = g_isClassicMode ? UP : (lastTimeButtonPressed == DOWN) ? lastTimeButtonPressed : UP; }
        else if (gb.buttons.repeat(BTN_RIGHT,1))
        {g_lastButtonPressed = g_isClassicMode ? RIGHT : (lastTimeButtonPressed == LEFT) ? lastTimeButtonPressed : RIGHT; }
        else if (gb.buttons.repeat(BTN_DOWN, 1))
        {g_lastButtonPressed = g_isClassicMode ? DOWN : (lastTimeButtonPressed == UP) ? lastTimeButtonPressed : DOWN; }
        else if (gb.buttons.repeat(BTN_LEFT, 1))
        { g_lastButtonPressed = g_isClassicMode ? LEFT : (lastTimeButtonPressed == RIGHT) ? lastTimeButtonPressed : LEFT; }
        else if (gb.buttons.repeat(BTN_B, 1) || gb.buttons.repeat(BTN_A, 1))
        {
          g_lastButtonPressed = PAUSE;
          if (lastTimeButtonPressed != g_lastButtonPressed)
            gb.popup(F("Pause"),9);
        }
        if (timeChanged)
          lastTimeButtonPressed = g_lastButtonPressed;

        drawSnake(g_lastButtonPressed, timeChanged);
        if (g_lastButtonPressed != PAUSE && timeChanged && snakeHasPrey())
            { setNewNomPos(); }
        drawPrey();
    }
}


/// set difficulty:
void setDifficulty()
{
    gb.display.clear();
    gb.display.setFont(font5x7);
    while(1){
        if(!gb.update())
            continue;
        
        if(gb.buttons.pressed(BTN_C) or gb.buttons.pressed(BTN_A))
            { return; }
        
        if(g_gameLevel > 1 and (gb.buttons.repeat(BTN_LEFT, 2) or gb.buttons.repeat(BTN_DOWN, 2)))
            g_gameLevel--;
        if(g_gameLevel < c_maxLevel and (gb.buttons.repeat(BTN_RIGHT, 2) or gb.buttons.repeat(BTN_UP, 2)))
            g_gameLevel++;
        
        gb.display.print(F("Difficulty: "));
        gb.display.print(g_gameLevel);
        
        uint8_t xPos = 8;
        uint8_t yPos = 40;
        for(uint8_t i = 0; i < c_maxLevel ; i++)
        {
            uint8_t currentHeight= (i+1)*4 + 1;
         
            gb.display.drawFastVLine(xPos+5, yPos, currentHeight + 1);
            gb.display.drawFastHLine(xPos, yPos+currentHeight+1, 6);
            
            if (i < g_gameLevel)
            { gb.display.fillRect(xPos, yPos,4,currentHeight); }
            
            xPos += 8;
            yPos -= 4;
        } 
    }
}


void showHighscore(bool useClassicMode)
{
    gb.display.setFont(font3x5);
    drawHighScores(useClassicMode);
}


void showCredits()
{
    gb.display.clear();
    gb.display.setFont(font5x7);
    while (true) {
        if (!gb.update())
            continue;
        gb.display.setFont(font3x5);
        gb.display.println(F("v2.0 Oct12"));
        gb.display.println(F("CC-BY-SA 2018"));
        gb.display.println(F("Lady Awesome"));
        gb.display.println(F("MakerSquirrel"));
        gb.display.println(F("  HighScore code"));
        gb.display.println(F("  by R0d0t"));
        if (gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)) {
            gb.sound.playOK();
            break;
        }
    }
}


/// triggered whenever we want to start the game
void initGame(bool useClassicMode) {
    gb.battery.show = false; //hide the battery indicator
    g_isClassicMode = useClassicMode;
    gb.pickRandomSeed(); //pick a different random seed each time for games to be different

    if (g_isClassicMode)
    {
        g_snake.reset();
        Coordinate::g_snakePos = Coordinate(8,c_rasterY-1);
        g_snake.addCoordinate(Coordinate::g_snakePos);
        g_snake.addCoordinate(Coordinate(7,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(6,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(5,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(4,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(3,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(2,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(1,c_rasterY-1));
        g_snake.addCoordinate(Coordinate(0,c_rasterY-1));
        Coordinate::g_nomPos = Coordinate(c_rasterX/2, c_rasterY/2);;
        g_lastButtonPressed = RIGHT;
    }
    else
    {
      g_snake.reset();
      Coordinate::g_snakePos = Coordinate(c_rasterX/2, c_rasterY/2);
      g_snake.addCoordinate(Coordinate::g_snakePos);
      g_lastButtonPressed = PAUSE;
      gb.display.setFont(font3x5);
      setNewNomPos();
      for(auto i = 0 ; i <= c_rasterX; i++)
        { g_xWallsRemoved[i] = false; }
      for(auto i = 0 ; i <= c_rasterY; i++)
        { g_yWallsRemoved[i] = false; }
      g_xWallsRemaining = c_rasterX+1;
      g_yWallsRemaining = c_rasterY+1;
    }
    g_nom = false;
    g_score = 0;
    g_levelModuloHelper = c_maxLevel + 1 - g_gameLevel;
    g_delayCounter = 0;
    if (c_debug)
        gb.display.setFont(font3x5);
}


//Based on code from Crabator, by Rodot
void loadHighscores(bool useClassicMode)
{
    int16_t offset = useClassicMode ? HIGHSCORE_COUNT * (NAME_LETTERS+3) : 0;
    auto& highScores = useClassicMode ? g_highScoresClassic : g_highscoreScores;
    auto& names = useClassicMode ? g_highScoreNamesClassic : g_highscoreNames;
    auto& minScore = useClassicMode ? g_minHighScoreClassic : g_minHighscore;
    for (uint8_t j = 0; j < HIGHSCORE_COUNT; j++) {
        for (uint8_t i = 0; i < NAME_LETTERS; i++)
            { names[j][i] = EEPROM.read(offset + i + j * (NAME_LETTERS + 2)); }
        highScores[j] = EEPROM.read(offset + NAME_LETTERS + j * (NAME_LETTERS + 2)) & 0x00FF; //LSB
        highScores[j] += (EEPROM.read(offset + NAME_LETTERS + 1 + j * (NAME_LETTERS + 2)) << 8) & 0xFF00; //MSB
        highScores[j] = (highScores[j] == 0xFFFF) ? 0 : highScores[j];
        minScore = highScores[j];
    }
}


void saveHighscore(bool useClassicMode)
{
    int16_t offset = useClassicMode ? HIGHSCORE_COUNT * (NAME_LETTERS+3) : 0;
    auto& highScores = useClassicMode ? g_highScoresClassic : g_highscoreScores;
    auto& names = useClassicMode ? g_highScoreNamesClassic : g_highscoreNames;
    auto& minScore = useClassicMode ? g_minHighScoreClassic : g_minHighscore;
    gb.getDefaultName(names[HIGHSCORE_COUNT - 1]);
    gb.keyboard(names[HIGHSCORE_COUNT - 1], NAME_LETTERS + 1);
    highScores[HIGHSCORE_COUNT - 1] = g_score;

    //Sort highscores
    for (uint8_t i = HIGHSCORE_COUNT - 1; i > 0; i--) {
        if (highScores[i - 1] >= highScores[i])
            { break; }
        char tempName[NAME_LETTERS];
        strcpy(tempName, names[i - 1]);
        strcpy(names[i - 1], names[i]);
        strcpy(names[i], tempName);
        unsigned int tempScore;
        tempScore = highScores[i - 1];
        highScores[i - 1] = highScores[i];
        highScores[i] = tempScore;

    }
    //update minimum highscore
    minScore = highScores[HIGHSCORE_COUNT-1];

    //Save highscores in EEPROM
    for (uint8_t j = 0; j < HIGHSCORE_COUNT; j++) {
        for (uint8_t i = 0; i < NAME_LETTERS; i++)
            { EEPROM.write(offset + i + j * (NAME_LETTERS + 2), names[j][i]); }
        EEPROM.write(offset + NAME_LETTERS + j * (NAME_LETTERS + 2), highScores[j] & 0x00FF); //LSB
        EEPROM.write(offset + NAME_LETTERS + 1 + j * (NAME_LETTERS + 2), (highScores[j] >> 8) & 0x00FF); //MSB
    }
}


void drawHighScores(bool useClassicMode) {
  gb.display.setFont(font3x5);
  const auto& highScores = useClassicMode ? g_highScoresClassic : g_highscoreScores;
  const auto& names = useClassicMode ? g_highScoreNamesClassic : g_highscoreNames;
  while (true) {
    if (!gb.update())
        continue;
    gb.display.drawRect(0, 0, LCDWIDTH, LCDHEIGHT);

    //Title
    gb.display.cursorX = useClassicMode ? 4 : 20;
    gb.display.cursorY = 3;
    if (useClassicMode)
        { gb.display.println(F("CLASSIC HIGH SCORES")); }
    else
        { gb.display.println(F("HIGH SCORES")); }

    gb.display.cursorY = gb.display.fontHeight * 2;
    for (uint8_t i = 0; i < HIGHSCORE_COUNT; i++) {
        gb.display.cursorX = 6;

        //Name
        if (highScores[i] == 0)
            { gb.display.print('-'); }
        else
            { gb.display.print(names[i]); }

        //Score
        if (highScores[i] > 9999) {
            gb.display.cursorX = LCDWIDTH - 6 - 5 * gb.display.fontWidth;
        } else if (highScores[i] > 999) {
            gb.display.cursorX = LCDWIDTH - 6 - 4 * gb.display.fontWidth;
        } else if (highScores[i] > 99) {
            gb.display.cursorX = LCDWIDTH - 6 - 3 * gb.display.fontWidth;
        } else if (highScores[i] > 9) {
            gb.display.cursorX = LCDWIDTH - 6 - 2 * gb.display.fontWidth;
        } else {
            gb.display.cursorX = LCDWIDTH - 6 - gb.display.fontWidth;
        }
        gb.display.cursorY = (gb.display.fontHeight * 2) + (gb.display.fontHeight * i);
        gb.display.println(highScores[i]);
    }

    if (gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C)) {
        gb.sound.playOK();
        break;
    }
  }
}


void drawArena()
{
  gb.display.drawRect(0, 0, LCDWIDTH, LCDHEIGHT);
  if (g_isClassicMode)
    return;
  gb.display.setColor(WHITE, WHITE);
  /// remove torn down wall parts
  if (g_xWallsRemoved[0])
  {
    gb.display.fillRect(0, 0, 5, 2);
    gb.display.fillRect(0, LCDHEIGHT-1, 5, 2);
  }
  if (g_yWallsRemoved[0])
  {
    gb.display.fillRect(0, 0, 2, 5);
    gb.display.fillRect(LCDWIDTH-1, 0, 2, 5);
  }
  for(uint8_t i = 1; i < c_rasterX; i++)
  {
    if (!g_xWallsRemoved[i]) continue;
    gb.display.fillRect(i*4+1, 0, 5, 2);
    gb.display.fillRect(i*4+1, LCDHEIGHT-1, 5, 2);
  }
  for(uint8_t i = 1; i < c_rasterY; i++)
  {
    if (!g_yWallsRemoved[i]) continue;
    gb.display.fillRect(0, i*4+1, 2, 5);
    gb.display.fillRect(LCDWIDTH-1, i*4+1, 2, 5);
  }
  gb.display.setColor(BLACK, WHITE);
  if (g_lastButtonPressed == PAUSE)
  {
    gb.display.cursorX = 2;
    gb.display.cursorY = 2;
    gb.display.print(g_score);
  }
  if (!c_debug)
    return;
  gb.display.cursorX = 1;
  gb.display.cursorY = 33;
  for(uint8_t i = 0; i < c_rasterX; i++)
  {
    if (!g_xWallsRemoved[i]) continue;
    gb.display.print(i);
  }
  gb.display.cursorX = 1;
  gb.display.cursorY = 39;
  for(uint8_t i = 0; i < c_rasterY; i++)
  {
    if (!g_yWallsRemoved[i]) continue;
    gb.display.print(i);
  }
}


void drawSnake(int8_t direction, bool timeChanged)
{
    if (timeChanged)
    {
      bool hasMoved = Coordinate::g_snakePos.move(direction, g_isClassicMode, isOutOfBounds(Coordinate::g_snakePos), Coordinate::g_snakePos.isInArena());
        if (hasMoved)
        {
            if (g_nom == true || g_growNom > 0)
            {
                g_snake.addCoordinate(Coordinate()); // dummy coordinate, is updated in moveCoordinates
                g_growNom -= g_growNom > 0 ? 1 : 0; // only count down if more than 0
                g_nom = false;
            }
            g_snake.moveCoordinates(Coordinate::g_snakePos);
            if (c_debug) 
            {
//                 g_cpuLoad = gb.getCpuLoad();
                g_ramUsage = gb.getFreeRam();
            }
        }            
    }
    
    if (c_debug) 
    {
        gb.display.cursorX = 10;
        gb.display.cursorY = 3;
//         gb.display.print(F("CPU/RAM: "));
//         // this itself does actually cost a lot of performance!
//         gb.display.print(g_cpuLoad);
//         gb.display.print(F("/"));
        gb.display.print(g_ramUsage);
    }
    uint8_t size = g_snake.size();
    for (int index = 0; index < size; index++) /// draw step
    {    
      const Coordinate& coord = g_snake.get(index);
      int8_t xPixel = coord.xPixel();
      int8_t yPixel = coord.yPixel();
      gb.display.drawFastHLine(xPixel, yPixel, 3);
      gb.display.drawFastHLine(xPixel, yPixel+1, 3);
      gb.display.drawFastHLine(xPixel, yPixel+2, 3);
      
      if (index > 0) /// polish step (fills gaps between the segments)
      {
        const Coordinate& otherCoord = g_snake.get(index-1);
        int8_t deltaX = coord.x() - otherCoord.x();
        int8_t deltaY = coord.y() - otherCoord.y();
        if (abs(deltaX) < 2 && abs(deltaY) < 2) /// skip parts where snake passes through wall gaps
        {
          gb.display.drawFastHLine(xPixel-deltaX, yPixel-deltaY, 3);
          gb.display.drawFastHLine(xPixel-deltaX, yPixel-deltaY+1, 3);
          gb.display.drawFastHLine(xPixel-deltaX, yPixel-deltaY+2, 3);
        }
      }
    }
}


void drawPrey()
{
  gb.display.drawCircle(Coordinate::g_nomPos.xPixel()+1, Coordinate::g_nomPos.yPixel()+1, 1);
  if (g_isClassicMode)
    return;
  if (Coordinate::g_shrinkNomPos.isInArena())
    gb.display.drawFastHLine(Coordinate::g_shrinkNomPos.xPixel(), Coordinate::g_shrinkNomPos.yPixel()+1, 3);
  if (Coordinate::g_growNomPos.isInArena())
  {
    gb.display.drawFastHLine(Coordinate::g_growNomPos.xPixel(), Coordinate::g_growNomPos.yPixel()+1, 3);
    gb.display.drawFastVLine(Coordinate::g_growNomPos.xPixel()+1, Coordinate::g_growNomPos.yPixel(), 3);
  }
  if (Coordinate::g_wallNomPos.isInArena())
    gb.display.drawRect(Coordinate::g_wallNomPos.xPixel(), Coordinate::g_wallNomPos.yPixel(), 3, 3);
}


/// New nompos is set to a place where no snake segment is found
void setNewNomPos()
{
  Coordinate::g_nomPos.setOffBounds(); // needed for to not interfere with new random free pos.
  Coordinate::g_nomPos = g_snake.getRandomFreePos();
  if (g_isClassicMode)
    return;
  Coordinate::g_shrinkNomPos.setOffBounds();
  Coordinate::g_growNomPos.setOffBounds();
  Coordinate::g_wallNomPos.setOffBounds();
  if (g_snake.size() < 8)
    return;
  uint8_t rngVal = random(0, 38);
  if (rngVal == 4 || rngVal == 2 || rngVal == 3 || rngVal == 33)
    Coordinate::g_shrinkNomPos = g_snake.getRandomFreePos();
  else if (rngVal == 23 || rngVal == 5 || rngVal == 7 || rngVal == 29)
    Coordinate::g_growNomPos = g_snake.getRandomFreePos();
  else if (rngVal == 13 || rngVal == 37 || rngVal == 1)
    Coordinate::g_wallNomPos = g_snake.getRandomFreePos();
}


/// returns true if any of the prey types have been eaten
bool snakeHasPrey()
{
  bool shrinkNom = false, wallNom = false;
  if (Coordinate::g_nomPos == Coordinate::g_snakePos)
  {
    g_nom = true; // will be set to false when growing is complete
    g_score+=g_gameLevel;
  }
  else if (!g_isClassicMode && Coordinate::g_shrinkNomPos == Coordinate::g_snakePos)
  {
    shrinkNom = true;
    g_snake.shrink(4);
    g_score-=4;
  }
  else if (!g_isClassicMode && Coordinate::g_growNomPos == Coordinate::g_snakePos)
  {
    g_growNom += 3; // will be count down to zero until growing is complete
    g_score+= 3*g_gameLevel;
  }
  else if (!g_isClassicMode && Coordinate::g_wallNomPos == Coordinate::g_snakePos)
  {
    wallNom = true;
    g_score+=g_gameLevel;
    deleteRandomWallElement();
  }
  
  if (g_nom || shrinkNom || wallNom || g_growNom == 3)
  {
    gb.sound.playNote(54, 1, 0);
    return true;
  }
  
  return false;
}


/// checks if snake runs into a wall
bool wallCollision()
{ return isOutOfBounds(Coordinate::g_snakePos); }


void gameOver(bool wallCrash)
  {
    gb.sound.playNote(0, 3, 0);

    if (wallCrash)
        gb.popup(F("Wallcrash!"),9);
    else
        gb.popup(F("Snakebite!"),9);

    if ((g_isClassicMode and (g_score > g_minHighScoreClassic)) or (!g_isClassicMode and (g_score > g_minHighscore)))
    {
      showScore();
      saveHighscore(g_isClassicMode);
      showHighscore(g_isClassicMode);
      return;
    }
    showScore();
    showHighscore(g_isClassicMode);
}


void showScore()
{
    gb.display.clear();
    gb.display.setFont(font5x7);
    while (true) {
        if (!gb.update())
            continue;
        gb.display.cursorY = 3;
        gb.display.cursorX = 14;
        gb.display.println(F("GAME OVER!"));
        gb.display.println(F(""));
        gb.display.cursorX = 12;
        gb.display.println(F("YOUR SCORE:"));
        gb.display.cursorX = 30;
        gb.display.println(g_score);
        gb.display.cursorX = 0;
        if ((g_isClassicMode and (g_score > g_minHighScoreClassic)) or (!g_isClassicMode and (g_score > g_minHighscore)))
            gb.display.println(F("NEW HIGHSCORE!"));
        if (gb.buttons.pressed(BTN_A) || gb.buttons.pressed(BTN_B) || gb.buttons.pressed(BTN_C))
            { break; }
    }
}


/// checks if snake bites itself
bool snakeBite()
{ return g_snake.isPartOfSnake(Coordinate::g_snakePos); }

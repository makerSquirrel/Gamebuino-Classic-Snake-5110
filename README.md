# Gamebuino-Classic-Snake-5110
A Snake game written for the GAMEBUINO Classic (https://gamebuino.com/) and the MAKERbuino (https://www.makerbuino.com/) 

Another Snake clone (made by Lady Awesome and MakerSquirrel, with modified highscore code found in a R0d0t game ;) ).
This version orients itself on Snake 1, which was running on the original Nokia 5110 (hence the name ;) )

Steering of the snake is done with the arrow keys, pausing the game can be done with A or B button, leaving the game with C button.
There are two game modes:

- In the standard mode the short snake only starts moving after clicking a direction and when steering backwards the snake does not die. Next to the normal stuff to eat for the snake, there are several new types:#
-- "GrowNoms" increase snake size by several bits, but bring more points,
-- "ShrinkNoms" reduce the snake size, but lowers score a bit and
-- "WallNoms": eating a piece removes a bit from the wall, which allows the snake passing through and entering at opposite side.
- The classic mode tries to mimic the original Snake version, where you start with the longer snake (which automatically starts when starting the game) and where steering backwards kills the Snake.

Since both versions are of different difficulty, they have different Highscores.  For both versions 9 different speed levels can be chosen, the higher the difficulty, the more points are gained when catching a dot with the snake.


Version history:

v1.0: Sep09-2018.

v2.0 Oct12-2018

New:

    Standard Mode got widely extended (see explanation above)
    Pause for A&B button (leave with C)
    current score is shown in pause mode
    fixed some bugs, which still allowed to kill oneself by pressing a series of arrow keys to fast.

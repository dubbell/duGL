#include <iostream>

#include "game.h"


int main()
{
    Game* game = new Game(1920, 1080);

    game->startMainLoop();
    game->stop();

    return 0;
}
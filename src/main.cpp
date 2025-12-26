#include <iostream>

#include "game.h"


int main()
{
    ExampleGame* game = new ExampleGame();

    game->startMainLoop();
    game->stop();

    return 0;
}
#include "dugl/game.h"

#include <iostream>

int main()
{
    ExampleGame* game = new ExampleGame();

    game->startMainLoop();
    game->stop();

    return 0;
}
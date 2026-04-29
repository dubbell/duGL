#include "dugl/env/simple_env.h"


int main()
{
    Environment* game = new ExampleEnvironment();

    game->startMainLoop();
    game->stop();

    return 0;
}
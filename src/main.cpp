#include "dugl/env/simple_env.h"


int main()
{
    Environment* env = new ExampleEnvironment();

    env->startMainLoop();
    env->stop();

    return 0;
}
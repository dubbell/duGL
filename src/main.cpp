#include "dugl/env/example_env.h"

#include <exception>
#include <iostream>


int main()
{
    try
    {
        Environment* env = new ExampleEnvironment();

        env->start();

        delete env;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << "Fatal error: an unknown exception was thrown." << std::endl;
        return 1;
    }

    return 0;
}
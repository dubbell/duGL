#include "dugl/env/example_physics_env.h"
#include "dugl/utils/jolt_adapter.h"

#include <exception>
#include <iostream>


int main()
{
    try
    {
        // The base Environment constructor needs Jolt's allocator registered before it can
        // build its AsyncDispatcher, regardless of whether this environment uses physics.
        initJolt();

        Environment* env = new ExamplePhysicsEnvironment();

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
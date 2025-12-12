#include <vertex_factory.h>


struct ColoredVertex
{
    float position[3];
    float color[3];
};

std::pair<unsigned int, std::vector<unsigned char>> VertexFactory::getColoredCube()
{
    std::vector<ColoredVertex> vertices = {
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},

        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},

        {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}},

        {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f}},
        {{0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}},
        {{0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f, -0.5f},  {0.0f, 1.0f, 0.0f}},
        {{0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}},
        {{0.5f,  0.5f,  0.5f},  {1.0f, 0.0f, 0.0f}},

        {{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},

        {{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f},  {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f},  {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f},  {1.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f},  {1.0f, 0.0f, 0.0f}}
    };

    const unsigned char* data_start = reinterpret_cast<const unsigned char*>(vertices.data());
    const unsigned char* data_end = data_start + (vertices.size() * sizeof(ColoredVertex));
    std::vector<unsigned char> data(data_start, data_end);

    return {36, data};
}

std::pair<unsigned int, std::vector<unsigned char>> VertexFactory::getGround()
{
    std::vector<ColoredVertex> vertices = {
        {{-100.0f, 0.0f, -100.0f}, {0.5f, 0.5f, 0.5f}},
        {{100.0f, 0.0f, -100.0f}, {0.5f, 0.5f, 0.5f}},
        {{100.0f, 0.0f, 100.0f}, {0.5f, 0.5f, 0.5f}},
        
        {{-100.0f, 0.0f, -100.0f}, {0.5f, 0.5f, 0.5f}},
        {{100.0f, 0.0f, 100.0f}, {0.5f, 0.5f, 0.5f}},
        {{-100.0f, 0.0f, 100.0f}, {0.5f, 0.5f, 0.5f}}
    };

    const unsigned char* data_start = reinterpret_cast<const unsigned char*>(vertices.data());
    const unsigned char* data_end = data_start + (vertices.size() * sizeof(ColoredVertex));
    std::vector<unsigned char> data(data_start, data_end);

    return {6, data};
}
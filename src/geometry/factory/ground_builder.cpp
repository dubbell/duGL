#include <ground_builder.h>




GroundBuilder::GroundBuilder(VertexManager* vertexManager) : VertexBuilder(vertexManager, 6) {}

std::array<float, 3> GroundBuilder::getVertexColor(std::array<float, 3> position)
{
    int cornerIndex = 0;
    if (position[0] > 0.0f) cornerIndex |= 1;
    if (position[2] > 0.0f) cornerIndex |= 2;

    return colors.at(cornerIndex % colors.size());
}

std::vector<std::array<float, 3>> GroundBuilder::getVertexPositions()
{
    return {
        {-100.0f, 0.0f, -100.0f},
        { 100.0f, 0.0f, -100.0f},
        { 100.0f, 0.0f,  100.0f},
        {-100.0f, 0.0f, -100.0f},
        { 100.0f, 0.0f,  100.0f},
        {-100.0f, 0.0f,  100.0f}};
}

std::vector<std::array<float, 2>> GroundBuilder::getVertexTextureCoords()
{
    return {
        {-10.0f, -10.0f},
        { 10.0f, -10.0f},
        { 10.0f,  10.0f},
        {-10.0f, -10.0f},
        { 10.0f,  10.0f},
        {-10.0f,  10.0f}};
}

Material GroundBuilder::getMaterial()
{
    return {
        { 0.4f, 0.3f, 0.3f },
        { 0.262f, 0.095f, 0.061f },
        { 0.35f, 0.35f, 0.35f },
        4.0f
    };
}
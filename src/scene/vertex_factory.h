#ifndef VERTEX_FACTORY_H
#define VERTEX_FACTORY_H

#include <vector>


class VertexFactory
{
public:
    static std::pair<unsigned int, std::vector<unsigned char>> getColoredCube();
    static std::pair<unsigned int, std::vector<unsigned char>> getGround();
};

#endif
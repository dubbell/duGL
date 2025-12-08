#ifndef VMANAGER_INTERFACE_H
#define VMANAGER_INTERFACE_H


class IVertexManager
{
public:
    virtual bool isLoaded(unsigned int VAO, unsigned int VBO) = 0;
};

#endif
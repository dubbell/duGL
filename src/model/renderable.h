#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <iostream>

#include "utils/stb_image.h"
#include "shading/shader.h"
#include "mesh.h"


class Renderable
{
public:
    Renderable(std::string directory, std::vector<Mesh> meshes);
    void draw(Shader* shader);

private:
    std::vector<Mesh> meshes;
    std::string directory;
};

#endif
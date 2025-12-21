#ifndef RENDERABLE_BUILDER_H
#define RENDERABLE_BUILDER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <stack>

#include "renderable.h"
#include "mesh.h"


class RenderableBuilder
{
public:
    RenderableBuilder(std::string path);
    Renderable build();

private:
    Assimp::Importer import;
    const aiScene* scene;
    std::string path, directory;
    std::vector<Mesh> meshes;
    std::vector<Texture> loadedTextures;

    void processNode(aiNode* node, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

#endif
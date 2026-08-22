#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>
#include <stack>

#include "renderable.h"
#include "mesh.h"


class RenderableBuilder
{
private:
    Assimp::Importer importer;
    const aiScene* scene;
    std::string path, directory;
    std::vector<Mesh> meshes;
    std::vector<Texture> loadedTextures;

public:
    RenderableBuilder(std::string path);
    Renderable build();

private:
    void processNode(aiNode* node, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>

#include "renderable_builder.h"
#include "mesh.h"


// Builds a renderable from a model file on disk, using assimp.
class ModelBuilder : public RenderableBuilder
{
private:
    Assimp::Importer importer;
    const aiScene* scene;
    std::string path, directory;
    std::vector<Texture> loadedTextures;

public:
    ModelBuilder(std::string path);

private:
    void generateMeshes() override;

    void processNode(aiNode* node, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

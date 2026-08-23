#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <string>

#include "renderable_builder.h"
#include "mesh.h"


// A texture loaded from the model's directory, kept so that materials sharing an image file
// share the GL texture object rather than loading it twice.
struct Texture
{
    dugl::uint id;
    std::string path;
};


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
    Material loadMaterial(aiMaterial* mat);
    dugl::uint loadTexture(aiMaterial* mat, aiTextureType type);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
};

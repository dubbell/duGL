#include "dugl/modelling/model_builder.h"

#include <assimp/postprocess.h>
#include "dugl/utils/glad_include.h"

#include <iostream>
#include <format>
#include <stdexcept>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

ModelBuilder::ModelBuilder(std::string path) : path(path), directory(path.substr(0, path.find_last_of('/')))
{
    // Read file, only triangles (aiProcess_Triangulate option). Normals are part of the
    // vertex format, so have assimp derive them for meshes that ship without any
    // (aiProcess_GenSmoothNormals leaves meshes that already have them alone)
    scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);

    // check for errors, incomplete data, etc.
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        throw std::runtime_error(std::format("Failed to load model '{}': {}", path, importer.GetErrorString()));
    }
}

void ModelBuilder::generateMeshes()
{
    processNode(scene->mRootNode, scene);
}

void ModelBuilder::processNode(aiNode* node, const aiScene* scene)
{
    // loop through and process each mesh in the node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // process all child nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

unsigned int textureFromFile(std::string filename, std::string directory)
{
    std::string path = directory + '/' + filename;

    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = nrChannels == 1 ? GL_RED : (nrChannels == 3 ? GL_RGB : GL_RGBA);

        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cerr << "Warning: failed to load texture '" << path << "'; using an untextured slot instead." << std::endl;
    }


    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return texture;
}

// Loads the first map of the given type, or 0 if the material has none. A material can
// declare several maps of one type, but only the first is used; blending multiple maps of
// the same type is a material-graph feature, not something a single factor can express.
dugl::uint ModelBuilder::loadTexture(aiMaterial* mat, aiTextureType type)
{
    if (mat->GetTextureCount(type) == 0) return 0;

    aiString str;
    mat->GetTexture(type, 0, &str);

    // reuse the texture if this image file has already been loaded for another material
    for (const Texture& loaded : loadedTextures)
    {
        if (loaded.path == str.C_Str()) return loaded.id;
    }

    Texture texture = { textureFromFile(str.C_Str(), directory), str.C_Str() };
    loadedTextures.push_back(texture);

    return texture.id;
}

Material ModelBuilder::loadMaterial(aiMaterial* mat)
{
    Material material;

    // factors are optional in most formats; keep the defaults for whatever isn't specified
    aiColor3D color;
    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
    {
        material.diffuseColor = { color.r, color.g, color.b };
    }
    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
    {
        material.specularColor = { color.r, color.g, color.b };
    }

    float shininess;
    // a zero exponent would make the specular term constant over the whole surface
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS && shininess > 0.0f)
    {
        material.shininess = shininess;
    }

    material.diffuseMap = loadTexture(mat, aiTextureType_DIFFUSE);
    material.specularMap = loadTexture(mat, aiTextureType_SPECULAR);

    return material;
}

Mesh ModelBuilder::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<StaticVertex> vertices;
    std::vector<unsigned int> indices;
    Material material;

    // a mesh isn't required to carry normals or texture coordinates, and assimp leaves those
    // arrays null when it has none. Every vertex still has to fill the whole vertex format,
    // so substitute defaults rather than reading through a null array.
    bool hasNormals = mesh->HasNormals();
    bool hasTexCoords = mesh->HasTextureCoords(0);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        StaticVertex vertex = {
            { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z },
            hasNormals
                ? glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z)
                : glm::vec3(0.0f, 1.0f, 0.0f),  // an arbitrary unit normal; lighting stays defined
            hasTexCoords
                ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y)
                : glm::vec2(0.0f, 0.0f)};
        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // mMaterialIndex is unsigned, so it needs bounds-checking against the scene's material
    // count rather than the '>= 0' test that reads naturally here
    if (mesh->mMaterialIndex < scene->mNumMaterials)
    {
        material = loadMaterial(scene->mMaterials[mesh->mMaterialIndex]);
    }

    return Mesh(std::move(vertices), std::move(indices), material);
}
#include <vertex_manager.h>

VertexManager::VertexManager() {};


unsigned int VertexManager::createAttributeObject()
{
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    vaoToVbos[VAO];
    configs[VAO];

    return VAO;
}

unsigned int VertexManager::createBufferObject(unsigned int VAO)
{
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    vaoToVbos.at(VAO).insert(VBO);
    vboToVao.insert({ VBO, VAO });
    contents[VBO];

    return VBO;
}

void VertexManager::addAttribute(unsigned int VAO, GLuint size, GLenum type, GLboolean normalized)
{
    VertexConfiguration& config = configs.at(VAO);
    config.attributes.push_back({ size, type, normalized });
    config.loaded = false;
}

void VertexManager::loadAttributes(unsigned int VAO)
{
    for (unsigned int VBO : vaoToVbos.at(VAO))
        loadAttributes(VAO, VBO);
    
    configs.at(VAO).loaded = true;
}

void VertexManager::loadAttributes(unsigned int VAO, unsigned int VBO)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    auto& configuration = configs.at(VAO);

    // deactivate and clear active attribute pointers
    for (auto active : configuration.activePointers)
        glDisableVertexAttribArray(active);
    configuration.activePointers.clear();

    // compute vertex stride
    GLsizei stride = 0;
    for (auto attr : configuration.attributes)
        stride += attr.size * sizeof(attr.type);

    // set and enable attribute pointers
    GLuint index = 0;
    size_t offset = 0;
    for (auto attr : configuration.attributes)
    {
        glVertexAttribPointer(index, attr.size, attr.type, attr.normalized, stride, (void*)offset);
        glEnableVertexAttribArray(index);
        index += 1;
        offset += attr.size * sizeof(attr.type);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Renderable* VertexManager::createRenderable(
    unsigned int VBO,
    std::vector<unsigned char> data,
    unsigned int vertexCount,
    GLenum usage,
    Shader* shader)
{
    VertexContent& content = contents.at(VBO);
    Renderable* renderable = content.renderables.emplace_back(
        std::make_unique<Renderable>(0, vertexCount, vboToVao[VBO], VBO, this, shader)).get();
    
    content.data.insert(content.data.end(), data.begin(), data.end());

    content.usage = usage;
    content.loaded = false;

    return renderable;
}

void VertexManager::loadVertexData(unsigned int VBO)
{
    VertexContent& content = contents.at(VBO);
    
    // load VBO data into VRAM
    glBindVertexArray(vboToVao[VBO]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, content.data.size() * sizeof(unsigned char), content.data.data(), content.usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // set renderable vertex start indices
    GLint accStartIndex = 0;
    for (const auto& renderable : content.renderables)
    {
        renderable->setStartIndex(accStartIndex);
        accStartIndex += renderable->getVertexCount();
    }

    content.loaded = true;
}

bool VertexManager::isLoaded(unsigned int VAO, unsigned int VBO)
{
    return configs.at(VAO).loaded && contents.at(VBO).loaded;
}
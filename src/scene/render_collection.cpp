#include <render_collection.h>



RenderCollection::RenderCollection(Shader* shader) 
    : shader(shader), vertexStride(0), totalSize(0), usage(GL_STATIC_DRAW)
{
    glGenVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VBO);
}

void RenderCollection::refreshAttributePtrs() const
{
    glBindVertexArray(VAO);

    int attrIndex = 0;
    size_t offset = 0;
    for (auto attr : attributes)
    {
        glVertexAttribPointer(attrIndex, attr.size, attr.type, attr.normalized, vertexStride, (void*)offset);
        glEnableVertexAttribArray(attrIndex);
        attrIndex += 1;
        offset = attr.size * sizeof(attr.type);
    }

    glBindVertexArray(0);
}

void RenderCollection::addVertexAttribute(GLuint size, GLenum type, GLboolean normalized)
{
    attributes.push_back(VertexAttribute(size, type, normalized));
    vertexStride += size * sizeof(type);
    refreshAttributePtrs();
}

void RenderCollection::createRenderable(std::string name, std::vector<float> vertexData)
{
    int vertexStart = static_cast<int>(data.size());
    int vertexDataSize = static_cast<int>(vertexData.size());
    Renderable renderable(vertexStart, vertexDataSize);
    renderables.insert({name, renderable});
    data.insert(data.end(), vertexData.begin(), vertexData.end());
    totalSize += sizeof(float) * vertexDataSize;
}

void RenderCollection::createEntity(std::string renderable, glm::vec3 &position)
{
    entities[renderable].push_back(Entity(position));
}

void RenderCollection::loadRemote() const
{
    const float* vertexData = data.data();
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, totalSize, vertexData, usage);
}

void RenderCollection::clearLocal()
{
    data.clear();
}

void RenderCollection::renderEntities(glm::mat4 &viewMatrix, glm::mat4 &projectionMatrix)
{
    glBindVertexArray(VAO);

    shader->use();

    shader->setMatrix("view", viewMatrix);
    shader->setMatrix("projection", projectionMatrix);

    for (auto iter = entities.begin(); iter != entities.end(); iter++)
    {
        Renderable renderable = renderables.at(iter->first);
        for (Entity entity : iter->second)
        {
            glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), entity.getPosition());
            shader->setMatrix("model", modelMatrix);
            glDrawArrays(GL_TRIANGLES, renderable.getStartIndex(), renderable.getVertexCount());
        }
    }

    glBindVertexArray(0);
}
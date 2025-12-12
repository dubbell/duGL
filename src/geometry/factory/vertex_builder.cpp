#include <vertex_builder.h>




VertexBuilder::VertexBuilder()
        : _VAO(0), _VBO(0), _usage(GL_STATIC_DRAW), enableTextures(false), enableColors(false)
{}

VertexBuilder::VertexBuilder(VertexManager* vertexManager, int numVertices)
        : _vertexManager(vertexManager), _VAO(0), _VBO(0), vertexCount(numVertices), _usage(GL_STATIC_DRAW), enableTextures(false), enableColors(false)
{}


// setters -----------

VertexBuilder* VertexBuilder::addTexture(std::string &texturePath)
{
    unsigned int texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + static_cast<GLint>(textures.size()));
    glBindTexture(GL_TEXTURE_2D, texture);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture." << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    textures.push_back(texture);
    enableTextures = true;

    return this;
}

VertexBuilder* VertexBuilder::addColors(std::vector<std::array<float, 3>> &newColors)
{
    colors.insert(colors.end(), newColors.begin(), newColors.end());
    enableColors = true;

    return this;
}

VertexBuilder* VertexBuilder::setVAO(unsigned int VAO)
{
    _VAO = VAO;
    return this;
}

VertexBuilder* VertexBuilder::setVBO(unsigned int VBO)
{
    _VBO = VBO;
    return this;
}

VertexBuilder* VertexBuilder::setUsage(GLenum usage)
{
    _usage = usage;
    return this;
}

VertexBuilder* VertexBuilder::setShader(Shader* shader)
{
    _shader = shader;
    return this;
}


std::vector<unsigned char> VertexBuilder::getData()
{
    std::vector<float> data(vertexCount * 3 + vertexCount * 2 * enableTextures + vertexCount * 3 * enableColors);

    auto positions = getVertexPositions();
    auto textureCoords = enableTextures ? getVertexTextureCoords() : std::vector<std::array<float, 2>>();

    for (size_t v_i = 0; v_i < vertexCount; v_i++)
    {
        size_t startIndex = v_i * (3 + 2 * enableTextures + 3 * enableColors);

        std::copy(
            positions.at(v_i).begin(),
            positions.at(v_i).end(),
            data.begin() + startIndex);

        if (enableTextures)
        {
            std::copy(
                textureCoords.at(v_i).begin(),
                textureCoords.at(v_i).end(),
                data.begin() + startIndex + 3);
        }

        if (enableColors)
        {
            auto color = getVertexColor(positions.at(v_i));
            std::copy(
                color.begin(),
                color.end(),
                data.begin() + startIndex + 3 + 2 * enableTextures);
        }
    }

    const unsigned char* dataStart = reinterpret_cast<const unsigned char*>(data.data());
    const unsigned char* dataEnd = dataStart + (data.size() * sizeof(float));

    return std::vector<unsigned char>(dataStart, dataEnd);
}


std::unique_ptr<Renderable> VertexBuilder::build()
{
    // must have corresponding shader
    if (_shader == nullptr) throw std::invalid_argument("vertex_builder : shader not specified");

    // set or create vertex attribute and buffer objects
    unsigned int VAO = _VAO == 0 ? _vertexManager->createAttributeObject() : _VAO;
    unsigned int VBO = _VBO == 0 ? _vertexManager->createBufferObject(VAO) : _VBO;
    
    // only set attributes if VAO was just created in vertex manager, otherwise they already exist
    if (_VAO == 0)
    {
        _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);  // position attribute
        if (enableTextures) _vertexManager->addAttribute(VAO, 2, GL_FLOAT, GL_FALSE);  // texture coord attribute
        if (enableColors) _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);  // color attribute
    }

    // generate data
    auto data = getData();

    // create renderable object
    auto renderable = std::unique_ptr<Renderable>(_vertexManager->createRenderable(VBO, data, vertexCount, _usage, _shader));

    // add textures
    for (unsigned int texture : textures)
        renderable->addTexture(texture);
    
    return renderable;
}
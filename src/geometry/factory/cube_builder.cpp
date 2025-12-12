#include <cube_builder.h>



CubeBuilder::CubeBuilder(VertexManager* vertexManager)
        : _vertexManager(vertexManager), _VAO(0), _VBO(0), _usage(GL_STATIC_DRAW), vertexCount(36), enableTextures(false), enableColors(false)
{}

VertexBuilder* CubeBuilder::addTexture(std::string &texturePath)
{
    unsigned int texture;
    glGenTextures(1, &texture);
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

    textures.push_back(texture);
    enableTextures = true;

    return this;
}

VertexBuilder* CubeBuilder::addColors(std::vector<std::vector<float>> &newColors)
{
    colors.insert(colors.end(), newColors.begin(), newColors.end());
    enableColors = true;

    return this;
}

VertexBuilder* CubeBuilder::setVAO(unsigned int VAO)
{
    _VAO = VAO;
    return this;
}

VertexBuilder* CubeBuilder::setVBO(unsigned int VBO)
{
    _VBO = VBO;
    return this;
}

VertexBuilder* CubeBuilder::setUsage(GLenum usage)
{
    _usage = usage;
    return this;
}


int CubeBuilder::getVertexCount()
{
    return vertexCount;
}

std::vector<unsigned char> CubeBuilder::getData()
{
    float cubePositionalData[36][3] = {
        {-0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f,  0.5f, -0.5f},
        {0.5f,  0.5f, -0.5f},
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},

        {-0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},

        {-0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f, -0.5f},
        {-0.5f, -0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},

        {0.5f,  0.5f,  0.5f},
        {0.5f,  0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f, -0.5f},
        {0.5f, -0.5f,  0.5f},
        {0.5f,  0.5f,  0.5f},

        {-0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f, -0.5f},
        { 0.5f, -0.5f,  0.5f},
        { 0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f,  0.5f},
        {-0.5f, -0.5f, -0.5f},

        {-0.5f,  0.5f, -0.5f},
        { 0.5f,  0.5f, -0.5f},
        { 0.5f,  0.5f,  0.5f},
        { 0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f,  0.5f},
        {-0.5f,  0.5f, -0.5f}
    };

    float cubeTextureCoordData[36][2] = {
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f},
        {0.0f, 1.0f},
        {1.0f, 1.0f},
        {1.0f, 0.0f},
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {0.0f, 1.0f}
    };
    
    std::vector<float> data(36 * 3 + 36 * 2 * enableTextures + 36 * 3 * enableColors);

    for (size_t v_i = 0; v_i < 36; v_i++)
    {
        size_t startIndex = v_i * (3 + 2 * enableTextures + 3 * enableColors);

        std::copy(
            std::begin(cubePositionalData[v_i]),
            std::end(cubePositionalData[v_i]),
            data.begin() + startIndex);

        if (enableTextures) std::copy(
            std::begin(cubeTextureCoordData[v_i]),
            std::end(cubeTextureCoordData[v_i]),
            data.begin() + startIndex + 3);

        if (enableColors)
        {
            int cubeCornerIndex = 0;
            if (cubePositionalData[v_i][0] > 0.0f) cubeCornerIndex |= 1;
            if (cubePositionalData[v_i][1] > 0.0f) cubeCornerIndex |= 2;
            if (cubePositionalData[v_i][2] > 0.0f) cubeCornerIndex |= 4;

            std::vector<float> colorVector = colors.at(cubeCornerIndex % colors.size());
            std::copy(
                colorVector.begin(),
                colorVector.end(),
                data.begin() + startIndex + 3 + 2 * enableTextures);
        }
    }

    const unsigned char* dataStart = reinterpret_cast<const unsigned char*>(data.data());
    const unsigned char* dataEnd = dataStart + (data.size() * sizeof(float));

    return std::vector<unsigned char>(dataStart, dataEnd);
}

std::unique_ptr<Renderable> CubeBuilder::build()
{
    unsigned int VAO = _VAO == 0 ? _vertexManager->createAttributeObject() : _VAO;
    unsigned int VBO = _VBO == 0 ? _vertexManager->createBufferObject() : _VBO;
    
    _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);  // position attribute

    if (enableTextures)
        _vertexManager->addAttribute(VAO, 2, GL_FLOAT, GL_FALSE);  // texture coord attribute
    if (enableColors)
        _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);  // color attribute

    auto data = getData();

    Shader* shader;  // TODO: implement shader manager

    return std::unique_ptr<Renderable>(_vertexManager->createRenderable(VBO, data, getVertexCount(), _usage, shader));
}
#include <vertex_builder.h>


// -------------- constructors ------------------

VertexBuilder::VertexBuilder() 
    : _VAO(0), _VBO(0),
      _usage(GL_STATIC_DRAW), 
      useColors(false),
      useTextures(false),
      useNormals(false),
      _shininess(8.0f),
      _diffuseMap(0),
      _specularMap(0)
{}

VertexBuilder::VertexBuilder(VertexManager* vertexManager, int numVertices)
    : _vertexManager(vertexManager), 
      vertexCount(numVertices), 
      _VAO(0), _VBO(0),
      _usage(GL_STATIC_DRAW), 
      useColors(false),
      useTextures(false),
      useNormals(false),
      _shininess(8.0f),
      _diffuseMap(0),
      _specularMap(0)
{}


// ------------- setup -----------

// coloring ---------------

VertexBuilder* VertexBuilder::addColors(std::vector<std::array<float, 3>> &newColors)
{
    colors.insert(colors.end(), newColors.begin(), newColors.end());
    useColors = true;

    return this;
}

// texturing ------------------

unsigned int VertexBuilder::createTextureMap(std::string& path)
{
    unsigned int textureMap;
    glGenTextures(1, &textureMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureMap);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nrChannels == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load diffuse map: " + path << std::endl;
    }

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(0);

    return textureMap;
}

VertexBuilder* VertexBuilder::addDiffuseMap(std::string& path)
{
    _diffuseMap = createTextureMap(path);
    useTextures = true;
    return this;
}

VertexBuilder* VertexBuilder::addSpecularMap(std::string& path)
{
    _specularMap = createTextureMap(path);
    useTextures = true;
    return this;
}


// other renderable attributes ------------

VertexBuilder* VertexBuilder::enableNormals()
{
    useNormals = true;
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

VertexBuilder* VertexBuilder::setShininess(float shininess)
{
    _shininess = shininess;
    return this;
}


// ------------- creation --------------

std::array<float, 3> VertexBuilder::getNormal(std::array<std::array<float, 3>, 3> positions)
{
    glm::vec3 right = {positions[1][0] - positions[0][0], positions[1][1] - positions[0][1], positions[1][2] - positions[0][2]};
    glm::vec3 left = {positions[2][0] - positions[0][0], positions[2][1] - positions[0][1], positions[2][2] - positions[0][2]};

    glm::vec3 normal = glm::normalize(glm::cross(left, right));

    return { normal.x, normal.y, normal.z };
}

std::vector<unsigned char> VertexBuilder::getData()
{
    std::vector<float> data(vertexCount * (3 + 2 * useTextures + 3 * useColors + 3 * useNormals));

    auto positions = getVertexPositions();
    auto textureCoords = useTextures ? getVertexTextureCoords() : std::vector<std::array<float, 2>>();
    std::array<float, 3> normal;

    for (size_t v_i = 0; v_i < vertexCount; v_i++)
    {
        size_t startIndex = v_i * (3 + 2 * useTextures + 3 * useColors + 3 * useNormals);

        std::copy(
            positions.at(v_i).begin(),
            positions.at(v_i).end(),
            data.begin() + startIndex);

        if (useTextures)
        {
            std::copy(
                textureCoords.at(v_i).begin(),
                textureCoords.at(v_i).end(),
                data.begin() + startIndex + 3);
        }

        if (useColors)
        {
            auto color = getVertexColor(positions.at(v_i));
            std::copy(
                color.begin(),
                color.end(),
                data.begin() + startIndex + 3 + 2 * useTextures);
        }

        if (useNormals)
        {
            // if new triangle
            if (v_i % 3 == 0)
            {
                auto triangleStartIndex = (v_i / 3) * 3;
                normal = getNormal({ 
                    positions.at(triangleStartIndex), 
                    positions.at(triangleStartIndex + 1), 
                    positions.at(triangleStartIndex + 2) });
            }
            std::copy(
                normal.begin(),
                normal.end(),
                data.begin() + startIndex + 3 + 2 * useTextures + 3 * useColors);
        }
    }

    const unsigned char* dataStart = reinterpret_cast<const unsigned char*>(data.data());
    const unsigned char* dataEnd = dataStart + (data.size() * sizeof(float));

    return std::vector<unsigned char>(dataStart, dataEnd);
}


std::unique_ptr<Renderable> VertexBuilder::build()
{
    // must specify shader
    if (_shader == nullptr) throw std::invalid_argument("vertex_builder : shader not specified");

    // set or create vertex attribute and buffer objects
    unsigned int VAO = _VAO == 0 ? _vertexManager->createAttributeObject() : _VAO;
    unsigned int VBO = _VBO == 0 ? _vertexManager->createBufferObject(VAO) : _VBO;
    
    // only set attributes if VAO was just created in vertex manager, otherwise they already exist
    if (_VAO == 0)
    {
        _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);                   // position attribute
        if (useTextures) _vertexManager->addAttribute(VAO, 2, GL_FLOAT, GL_FALSE);  // texture coord attribute
        if (useColors) _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);    // color attribute
        if (useNormals) _vertexManager->addAttribute(VAO, 3, GL_FLOAT, GL_FALSE);   // normal attribute
    }

    // generate data
    auto data = getData();

    // create renderable object
    auto renderable = std::unique_ptr<Renderable>(_vertexManager->createRenderable(VBO, data, vertexCount, _usage, _shader));

    // set material
    renderable->setMaterial({ _diffuseMap, _specularMap, _shininess });
    
    return renderable;
}
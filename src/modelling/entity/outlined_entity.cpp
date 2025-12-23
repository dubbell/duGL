#include "outlined_entity.h"



OutlinedEntity::OutlinedEntity(Renderable* renderable, Shader* outlineShader) 
    : Entity(renderable), outlineShader(outlineShader) {}

OutlinedEntity::OutlinedEntity(Renderable* renderable, glm::vec3 position, Shader* outlineShader) 
    : Entity(renderable, position), outlineShader(outlineShader) {}


void OutlinedEntity::render(Shader* shader)
{
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);

    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);  // keep value in stencil buffer or replace it on test result, for when 
                                                // stencil test fails, stencil test passes but depth test fails, and finally when both succeed
    glStencilFunc(GL_ALWAYS, 1, 0xFF);          // always write 1 to stencil where fragments are rendered
    glStencilMask(0xFF);                        // enable writing to the stencil buffer

    glm::mat4 modelTransform = getModelTransform();

    shader->use();
    shader->setMat4("model", modelTransform);

    renderable->render(shader);                 // render objects with the normal object shader while also writing to stencil buffer

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);        // stencil test passes if stencil buffer value not 1, i.e. not in position drawn previously
    glStencilMask(0x00);                        // disable writing to the stencil buffer

    outlineShader->use();
    outlineShader->setFloat("thickness", 0.15f);
    outlineShader->setMat4("model", modelTransform);
    
    renderable->render(outlineShader);          // draw outline (upscaled renderable)
    
    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
}
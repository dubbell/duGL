#include "outlined_entity.h"



OutlinedEntity::OutlinedEntity(Model* model, Shader* outlineShader) 
    : Entity(model), outlineShader(outlineShader) {}

OutlinedEntity::OutlinedEntity(Model* model, glm::vec3 position, Shader* outlineShader) 
    : Entity(model, position), outlineShader(outlineShader) {}


void OutlinedEntity::render()
{
    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // glStencilFunc(GL_ALWAYS, 1, 0xFF);
    // glStencilMask(0xFF);

    // decoratedEntity->render();

    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilMask(0x00);
    // glDisable(GL_DEPTH_TEST);

    // Shader* previousShader = BindingTracker::boundShader;
    // outlineShader->use();
}
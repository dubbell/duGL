#include "dugl/mouse.h"


MouseController::MouseController(PlayerInterface* screenCastInterface)
    : screenCastInterface(screenCastInterface), lastX(0.0f), lastY(0.0f), sensitivity(0.05f), firstMouse(true)
{}


void MouseController::registerOffsetObserver(MouseOffsetObserver* observer)
{
    mouseOffsetObservers.insert(observer);
}

void MouseController::unregisterOffsetObserver(MouseOffsetObserver* observer)
{
    mouseOffsetObservers.erase(observer);
}

void MouseController::registerScreenRayObserver(ScreenRayObserver* observer)
{
    screenRayObservers.insert(observer);
}

void MouseController::unregisterScreenRayObserver(ScreenRayObserver* observer)
{
    screenRayObservers.erase(observer);
}

void MouseController::handleCursorPosition(float xPos, float yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = (xPos - lastX) * sensitivity;
    float yOffset = (lastY - yPos) * sensitivity; 
    lastX = xPos;
    lastY = yPos;

    for (auto& observer : mouseOffsetObservers)
        observer->cursorOffsetCallback(xOffset, yOffset);
}

void MouseController::handleScreenRay(float xPos, float yPos)
{
    Camera* camera = screenCastInterface->getActiveCamera();
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();
    auto [viewportWidth, viewportHeight] = screenCastInterface->getViewportSize();
    
    glm::vec3 origin = camera->getPosition();
    glm::vec3 direction = castScreenRay(xPos, yPos, viewportWidth, viewportHeight, view, projection);

    for (auto& observer : screenRayObservers)
        observer->observeRay(origin, direction);
}

void MouseController::processMouseInput()
{
    double xPos, yPos;
    glfwGetCursorPos(screenCastInterface->getWindow(), &xPos, &yPos);

    handleCursorPosition(xPos, yPos);

    if (screenCastInterface->getFreeCursor())
        handleScreenRay(xPos, yPos);
}

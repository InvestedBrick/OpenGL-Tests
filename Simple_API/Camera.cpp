#include "Camera.hpp"
#include "defines.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp> 

glm::mat4 Camera::get_view_mat() const
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
}

glm::mat4 Camera::get_projection_mat() const
{
    float half_width = WINDOW_WIDTH * 0.5f / zoomLevel;
    float half_height = WINDOW_HEIGHT * 0.5f / zoomLevel;
    return glm::ortho(-half_width, half_width, -half_height, half_height, -1.0f, 1.0f);
}

void Camera::move_by(const glm::vec2 &offset)
{
    position += offset;
}

void Camera::update(bool W_Pressed, bool A_Pressed, bool S_Pressed, bool D_Pressed, bool Up_Pressed, bool Down_Pressed)
{
    float cameraSpeed = 0.5f;
    
    // Move camera based on key states
    if (W_Pressed) {
        position.y -= cameraSpeed * (7.f / zoomLevel);
    }
    if (A_Pressed) {
        position.x += cameraSpeed * (7.f / zoomLevel);
    }
    if (S_Pressed) {
        position.y += cameraSpeed * (7.f / zoomLevel);
    }
    if (D_Pressed) {
        position.x -= cameraSpeed * (7.f / zoomLevel);
    }
    if (Up_Pressed) {
        zoomLevel += cameraSpeed * (zoomLevel / 10.f);
    }
    if (Down_Pressed && zoomLevel > cameraSpeed * 2) {
        zoomLevel -= cameraSpeed * (zoomLevel / 10.f);

    }
}

    
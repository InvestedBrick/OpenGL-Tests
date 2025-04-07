#pragma once
#include <defines.hpp>
#include <glm/glm.hpp>
class Camera {
private:
    glm::vec2 position = glm::vec2(0.0f, 0.0f);
    float zoomLevel = 50.f;
public:
    glm::mat4 get_view_mat() const;
    glm::mat4 get_projection_mat() const;
    void move_by(const glm::vec2& offset);
    glm::vec2& get_position() { return position; }
    float& get_zoom() { return zoomLevel; }
    void set_position(const glm::vec2& pos) { position = pos; }
    void set_zoom(float zoom) { zoomLevel = zoom; }
    void update(bool W_Pressed, bool A_Pressed, bool S_Pressed, bool D_Pressed, bool Up_Pressed, bool Down_Pressed);
    
}; 
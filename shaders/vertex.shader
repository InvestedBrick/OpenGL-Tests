#version 430 core
layout(location = 0) in vec2 position;  
uniform mat4 u_projection;
uniform mat4 u_view;

out vec2 fragPosition; 

void main() {

    fragPosition = position;

    gl_Position = u_projection * u_view * vec4(position, 0.0, 1.0);
}

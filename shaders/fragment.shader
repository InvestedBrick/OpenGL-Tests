#version 330 core
layout(location = 0) out vec4 color;

uniform vec4 u_color;
void main(){
    // R, G, B, A
    color = u_color;
}
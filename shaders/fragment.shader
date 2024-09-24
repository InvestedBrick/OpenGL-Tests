#version 430 core
layout(location = 0) out vec4 color;

uniform vec4 u_color;
in vec2 theColor;
void main(){
    // R, G, B, A
    color = vec4(theColor.xy,u_color.zw);
}
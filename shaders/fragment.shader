#version 430 core
layout(location = 0) out vec4 color;

uniform vec4 u_color;
in vec2 theColor;
void main(){
    // R, G, B, A
    float blue = (theColor.x + theColor.y) / 2;

    // Ensure the blue value doesn't fall below a certain threshold
    blue = max(blue, 0.2);
    color = vec4(theColor.xy,blue,u_color.w);
}
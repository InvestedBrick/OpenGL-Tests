#version 430 core
layout(location = 0) out vec4 color;

uniform vec4 u_color;  

in vec2 fragPosition;  
void main() {


    float blue = (fragPosition.x + fragPosition.y) / 2.0;

    blue = max(blue, 0.2);

    color = vec4(fragPosition.xy, blue, u_color.w);
}

#version 430 core
layout(location = 0) out vec4 color;


in vec2 fragPosition;  
void main() {




    color = vec4(abs(fragPosition.xy), 0.2, 1.0);
}

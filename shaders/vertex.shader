#version 430 core
layout(location = 0) in vec4 position;

out vec2 theColor;
void main(){
    gl_Position = position;
    theColor = position.xy;
}
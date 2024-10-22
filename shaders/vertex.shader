#version 430 core
layout(location = 0) in vec2 position;  
layout(location = 1) in float velocity;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec4 fragColor; 

void main() {
    gl_Position = u_projection * u_view * vec4(position, 0.0, 1.0);

    float c = clamp(velocity / 1, 0.0, 1.0);

    vec3 color_low = vec3(0.0,0.0,1.0);
    vec3 color_high = vec3(1.0,0.0,0.0);

    fragColor = vec4(mix(color_low,color_high,c),1.0);
}

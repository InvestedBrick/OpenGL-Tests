#version 430 core

uniform mat4 u_projection;
uniform mat4 u_view;

layout (location = 0) in vec2 aPos; // Unit circle geometry


struct CircleData {
    float mass;
    float color; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      
};


layout (std430, binding = 0) buffer DataBuffer {
    CircleData data[];
};



const float MASS_TO_RADIUS = 0.0001; // Converts mass to radius for collision detection


out vec3 vColor; // Pass color to fragment shader

vec3 getColor(float c) {
    if (c < 0.25) {
        return mix(vec3(0.0, 0.0, 1.0), vec3(0.5, 0.5, 1.0), c / 0.25);
    } else if (c < 0.5) {
        return mix(vec3(0.5, 0.5, 1.0), vec3(1.0, 1.0, 0.0), (c - 0.25) / 0.25);
    } else if (c < 0.75) {
        return mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.5, 0.0), (c - 0.5) / 0.25);
    } else {
        return mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 0.0, 0.0), (c - 0.75) / 0.25);
    }
}

void main() {

    CircleData c = data[gl_InstanceID];
    vec2 worldPos = c.pos + aPos * c.mass * MASS_TO_RADIUS;
    gl_Position =  u_projection * u_view * vec4(worldPos, 0.0, 1.0);

    vColor = getColor(c.color);
}
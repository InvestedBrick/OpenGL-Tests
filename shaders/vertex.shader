#version 430 core
layout(location = 0) in vec2 position;  
layout(location = 1) in float velocity;
uniform mat4 u_projection;
uniform mat4 u_view;

out vec4 fragColor; 

vec3 getColor(float c) {
    if (c < 0.25) {
        // Transition from dark blue to lighter blue
        return mix(vec3(0.0, 0.0, 1.0), vec3(0.5, 0.5, 1.0), c / 0.25);
    } else if (c < 0.5) {
        // Transition from lighter blue to yellow
        return mix(vec3(0.5, 0.5, 1.0), vec3(1.0, 1.0, 0.0), (c - 0.25) / 0.25);
    } else if (c < 0.75) {
        // Transition from yellow to orange
        return mix(vec3(1.0, 1.0, 0.0), vec3(1.0, 0.5, 0.0), (c - 0.5) / 0.25);
    } else {
        // Transition from orange to red
        return mix(vec3(1.0, 0.5, 0.0), vec3(1.0, 0.0, 0.0), (c - 0.75) / 0.25);
    }
}

void main() {
    gl_Position = u_projection * u_view * vec4(position, 0.0, 1.0);

    // Normalize velocity to the range [0, 1]
    float c = clamp(velocity / 2, 0.0, 1.0);

    fragColor = vec4(getColor(c), 1.0);
}

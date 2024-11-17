#version 430
layout (local_size_x = 64) in;

struct CircleData {
    float mass;
    float debug_info; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      
};

const float G = 0.00005;
const float MASS_TO_RADIUS = 0.0001; // Converts mass to radius for collision detection
const float dt = 1.0 / 60.0;
layout (std430, binding = 0) buffer DataBuffer {
    CircleData data[];
};

void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index >= data.length()) {
        return;
    }

    precision highp float;

    data[index].force = vec2(0.0, 0.0);

    for (uint i = 0; i < data.length(); ++i) {
        if (i != index) {
            vec2 direction = data[i].pos - data[index].pos; 
            float dist = length(direction);
            float combined_radius = data[index].mass * MASS_TO_RADIUS + data[i].mass * MASS_TO_RADIUS;

            // Gravitational force calculation
            if (dist > combined_radius) { // Apply gravity only if not colliding
                float force_magnitude = (G * data[index].mass * data[i].mass) / (dist * dist);
                force_magnitude = min(force_magnitude, 0.5); // Limit force magnitude
                vec2 force = (direction / dist) * force_magnitude;
                data[index].force += force;
            } 

            // Collision detection
            if (dist <= combined_radius && dist > 0.0) {
                // Collision normal
                vec2 collision_normal = normalize(direction);

                data[index].vel -= collision_normal * 0.01;
            }
        }
    }

    data[index].vel += (data[index].force / data[index].mass) * dt;

    // Position update is NOT performed yet
    // data[index].pos += data[index].vel * dt; // Commented out
}

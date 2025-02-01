#version 430
layout (local_size_x = 64) in;

uniform uint vertices_per_circ;

struct CircleData {
    float mass;
    float color; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      
};

struct Vertex {
    vec2 pos;
};

const float G = 0.0015;
const float MASS_TO_RADIUS = 0.0001; // Converts mass to radius for collision detection
const float dt = 1.0 / 30.0;
const float TWO_PI = 6.2831853;

layout (std430, binding = 0) buffer DataBuffer {
    CircleData data[];
};

layout (std430, binding = 1) buffer VertexBuffer {
    Vertex vertices[];
};

layout (std430, binding = 2) buffer CosSinBuffer{
    vec2 cos_sin_precalcs[];
};

void calc_positions(uint idx) {
    uint base_idx = idx * vertices_per_circ;
    vertices[base_idx].pos = vec2(data[idx].pos);
    uint pos_idx = 1;
    float rad = data[idx].mass * MASS_TO_RADIUS;
    for (int i = 0; i < vertices_per_circ - 1; ++i) {
        vec2 xy = rad * cos_sin_precalcs[i];
        vertices[base_idx + pos_idx].pos = data[idx].pos + xy;
        pos_idx++;
    }
}


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
            float dist = sqrt(direction.x * direction.x + direction.y * direction.y);
            //float combined_radius = data[index].mass * MASS_TO_RADIUS + data[i].mass * MASS_TO_RADIUS;

            // Gravitational force calculation
            if (dist > 0.0) { // Apply gravity only if not colliding
                float force_magnitude = (G * data[index].mass * data[i].mass) / (dist * dist);
                force_magnitude = min(force_magnitude, 0.01); // Limit force magnitude
                vec2 force = (direction / dist) * force_magnitude;
                data[index].force += force;
            } 

            /*
            // Collision detection
            if (dist <= combined_radius && dist > 0.0) {
                // Collision normal
                vec2 collision_normal = normalize(direction);

                data[index].vel -= collision_normal * 0.04;
            }
            */
            
        }
    }
    
    data[index].vel += (data[index].force / data[index].mass) * dt;

    data[index].pos += data[index].vel * dt; 

    calc_positions(index);

    data[index].color = clamp(sqrt(data[index].vel.x * data[index].vel.x + data[index].vel.y * data[index].vel.y) / 2.0, 0.0, 1.0);

}

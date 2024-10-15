#version 430
layout (local_size_x = 64) in; 

struct CircleData {
    float mass;
    float debug_info; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      

};
const float G = 0.001;
const float mass_to_radius = 0.0001;
const float dt = 1.0 / 60.0;
layout (std430, binding = 0) buffer DataBuffer {
    CircleData data[];
};

void main() {
    uint index = gl_GlobalInvocationID.x; 
    precision highp float;

    if (index >= data.length()){
        return;
    }
    data[index].force = vec2(0.0,0.0);
    for (uint i = 0; i < data.length(); ++i) {
        if (i != index) {
            vec2 force = vec2(0.0,0.0);
            vec2 direction = data[i].pos - data[index].pos; 
            float dist = sqrt(direction.x * direction.x + direction.y * direction.y);
            if (dist > (data[index].mass * mass_to_radius + data[i].mass * mass_to_radius)) {
                float force_magnitude = (G * data[index].mass * data[i].mass) / (dist * dist);
                force_magnitude = min(force_magnitude,0.01);
                force = (direction / dist) * force_magnitude; 
            }
            data[index].force += force;
        }
    }

    data[index].vel += (data[index].force / data[index].mass) * dt;

}

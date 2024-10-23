#version 430
layout (local_size_x = 64) in; 

struct CircleData {
    float mass;
    float debug_info; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      

};
const float G = 0.000001;
const float MASS_TO_RADIUS = 0.0001;
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
            if (dist > data[index].mass * MASS_TO_RADIUS + data[i].mass * MASS_TO_RADIUS) {
                float force_magnitude = (G * data[index].mass * data[i].mass) / (dist * dist);
                force = (direction / dist) * force_magnitude; 
            }
            data[index].force += force;
        }
    }

    data[index].vel += (data[index].force / data[index].mass) * dt;

}

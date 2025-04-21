#version 430
layout (local_size_x = 64) in;


struct CircleData {
    float mass;
    float color; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      
};


const float G = 0.0015;
const float MASS_TO_RADIUS = 0.0001; // Converts mass to radius for collision detection
const float dt = 1.0 / 30.0;
const float TWO_PI = 6.2831853;

layout (std430, binding = 0) buffer DataBuffer {
    CircleData data[];
};



void main() {
    uint index = gl_GlobalInvocationID.x;

    if (index >= data.length()) {
        return;
    }

    data[index].force = vec2(0.0, 0.0);
    
    for (uint i = 0; i < data.length(); ++i) {
        if (i != index) {
            vec2 direction = data[i].pos - data[index].pos; 
            float dist = sqrt(direction.x * direction.x + direction.y * direction.y);
            //float combined_radius = data[index].mass * MASS_TO_RADIUS + data[i].mass * MASS_TO_RADIUS;

            // Gravitational force calculation
            if (dist > 0.0) {
                float force_magnitude = (G * data[index].mass * data[i].mass) / (dist * dist);
                force_magnitude = min(force_magnitude, 0.1); // Limit force magnitude
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

    //calc_positions(index);

    data[index].color = clamp(sqrt(data[index].vel.x * data[index].vel.x + data[index].vel.y * data[index].vel.y) / 2.0, 0.0, 1.0);

}

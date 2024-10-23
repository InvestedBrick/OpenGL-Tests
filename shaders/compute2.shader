#version 430
layout (local_size_x = 64) in;

struct CircleData {
    float mass;
    float debug_info; 
    vec2 pos;     
    vec2 vel;
    vec2 force;      

};

layout(std430) struct Quad{
    vec2 center;
    float size;
    uint n_objects;
};

layout(std430) struct Node{
    vec2 center_of_mass;
    float mass;
    uint children;
    uint next;
    Quad quad;
};
const float G = 0.00001;
const float MASS_TO_RADIUS = 0.00001;
const float dt = 1.0 / 60.0;
const float th_sq = 0.5 * 0.5;

layout (std430, binding = 0) buffer CircleBuffer {
    CircleData data[];
};

layout (std430, binding = 1) buffer NodeBuffer {
    Node nodes[];
};

float get_dist_squared(const vec2 a, const vec2 b){
    float x_sq = (b.x - a.x) * (b.x - a.x);
    float y_sq = (b.y - a.y) * (b.y - a.y);
    return  x_sq + y_sq;
}

vec2 calc_force(uint index,uint node){
    vec2 force = vec2(0.0,0.0);
    vec2 direction = nodes[node].center_of_mass - data[index].pos; 
    float dist = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (dist > data[index].mass * MASS_TO_RADIUS + nodes[node].mass * MASS_TO_RADIUS) {
        float force_magnitude = (G * data[index].mass * nodes[node].mass) / (dist * dist);
        force = (direction / dist) * force_magnitude; 
    }
    return force;
}


void main(){
    uint index = gl_GlobalInvocationID.x; 
    precision highp float;

    if (index >= data.length()){
        return;
    }

    data[index].force = vec2(0.0,0.0);
    uint node = 0;

    while(true){
        Node n = nodes[node];
        if (n.children == 0 || n.quad.size * n.quad.size < get_dist_squared(n.center_of_mass,data[index].pos) * th_sq){
            data[index].force += calc_force(index,node);

            if (n.next == 0){
                break;
            }
            node = n.next;
        }else{
            node = n.children;
        }
    }
    data[index].debug_info = node;
    data[index].vel += (data[index].force / data[index].mass) * dt;
}

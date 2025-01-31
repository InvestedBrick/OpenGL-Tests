#version 430 core
//layout (location = 0) in int vertexID;

uniform mat4 u_projection;
uniform mat4 u_view;
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

layout (std430, binding = 0) buffer DataBuffer {
    CircleData data[];
};

layout (std430, binding = 1) buffer VertexBuffer {
    Vertex vertices[];
};




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

    gl_Position = u_projection * u_view * vec4(vertices[gl_VertexID].pos, 0.0, 1.0);
    uint data_idx = gl_VertexID / vertices_per_circ;
    
    vColor = getColor(data[data_idx].color);
    //vColor = vec3(1.0,0.0,0.0);
}
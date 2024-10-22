#include "Quadtree.hpp"
#include <iostream>
#include <limits>
void Quadtree::reset(Quad quad)
{
    nodes.clear();
    parents.clear();
    nodes.reserve(quad.n_objects * 2);
    parents.reserve(quad.n_objects * 0.5);
    nodes.emplace_back(0,quad);
}

void Quadtree::insert_body(const Circle_Object& object)
{
    uint node = 0; // root
    while(!this->nodes[node].is_leaf()){
        uint q = nodes[node].quad.get_quadrant(object.pos);
        node = nodes[node].children + q;
    }

    if (nodes[node].mass == 0.0f){ // empty
        nodes[node].mass = object.mass;
        nodes[node].center_of_mass = object.pos;
        return;
    }

    const vec2f p = nodes[node].center_of_mass;
    const float m = nodes[node].mass;

    if (object.pos == p){
        nodes[node].mass += object.mass;
        return;
    }

    while (true){
        uint children = subdivide(node);

        uint q1 = nodes[node].quad.get_quadrant(p);
        uint q2 = nodes[node].quad.get_quadrant(object.pos);

        if (q1 == q2){
            node = children + q1;
        }else{
            uint n1 = children + q1;
            uint n2 = children + q2;

            nodes[n1].center_of_mass = p;
            nodes[n1].mass = m;
            nodes[n2].center_of_mass = object.pos;
            nodes[n2].mass = object.mass;
            return;
        }

    }
}
vec2f Quadtree::calc_force(const Circle_Object &object, uint node)
{
    vec2f force{0.0f,0.0f};
    vec2f r_vector = nodes[node].center_of_mass - object.pos;

    float distance = sqrt(r_vector.x * r_vector.x + r_vector.y * r_vector.y);

    if (distance > 0.0){
        float force_mag = GRAVITATIONAL_CONSTANT * object.mass * nodes[node].mass / (distance * distance);
        const float max_force = 0.1f;
        if (force_mag > max_force){
            force_mag = max_force;
        }
        force = (r_vector / distance) * force_mag;

    }
    return force;
}

Quad Quad::initialize(std::vector<Circle_Object> &objects)
{
    n_objects = objects.size();
    float min_x = std::numeric_limits<float>::max();
    float min_y = std::numeric_limits<float>::max();
    float max_x = std::numeric_limits<float>::lowest();
    float max_y = std::numeric_limits<float>::lowest();

    uint n_objects = objects.size();
    for(const auto& obj : objects){
        min_x = std::min(min_x, obj.pos.x);
        min_y = std::min(min_y, obj.pos.y);
        max_x = std::max(max_x, obj.pos.x);
        max_y = std::max(max_y, obj.pos.y);
    }

    float size = std::max(max_x - min_x, max_y - min_y);
    vec2f center{min_x + (size / 2),min_y + (size / 2)};

    return {center,size};
}

uint Quadtree::subdivide(uint node) {
    parents.emplace_back(node);
    const uint children = nodes.size();
    nodes[node].children = children;

    uint nexts[4] = {
        children + 1,
        children + 2,
        children + 3,
        nodes[node].next
    };

    std::array<Quad,4> quads = nodes[node].quad.subdivide();
    for (uint i = 0; i < 4; i++){
        nodes.emplace_back(nexts[i],quads[i]);
    }
    return children;
}
void Quadtree::update_mass_centers()
{
    for (std::vector<uint>::reverse_iterator r_i = parents.rbegin();
        r_i != parents.rend(); ++r_i){
        const uint ch = nodes[*r_i].children;

        nodes[*r_i].center_of_mass = nodes[ch].center_of_mass * nodes[ch].mass \
            + nodes[ch + 1].center_of_mass * nodes[ch + 1].mass \
            + nodes[ch + 2].center_of_mass * nodes[ch + 2].mass \
            + nodes[ch + 3].center_of_mass * nodes[ch + 3].mass;

        nodes[*r_i].mass =  nodes[ch].mass \
            + nodes[ch + 1].mass \
            + nodes[ch + 2].mass \
            + nodes[ch + 3].mass;

        nodes[*r_i].center_of_mass /= nodes[*r_i].mass;
    }
}
void Quadtree::apply_force(Circle_Object &object)
{
    if (nodes.empty()){
        object.force = vec2f{0.0f,0.0f};
        return;
    }

    uint node = 0; // root 

    while (true){
        Node n = nodes[node];

        if (n.is_leaf() || n.quad.size * n.quad.size < get_dist_squared(n.center_of_mass,object.pos) * th_sq){
           object.force += calc_force(object,node);
            if (n.next == 0){
                break;
            }
            node = n.next;
        }else{
            node = n.children;
        }
    }


}

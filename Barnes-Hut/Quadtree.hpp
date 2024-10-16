#pragma once
#include <memory>
#include <vector>
#include <cmath>
#include <array>
#include "../Simple_API/defines.hpp"
#include "../Circle_Object.hpp"
struct Quad{
    Quad(vec2f center_,float size_){
        center = center_;
        size = size_;
    }
    Quad() = default;
    vec2f center{0.0,0.0};
    float size;
    Quad initialize(std::vector<Circle_Object> &objects);
    Quad into_quads(int quadrant) const {
        Quad result = *this;
        result.size *= 0.5f;
        // Determine horizontal shift based on quadrant (0 or 1)
        float x_shift = (quadrant % 2 == 0) ? -0.5f : 0.5f;
        result.center.x += x_shift * result.size;
        // Determine vertical shift based on quadrant (0 or 2)
        float y_shift = (quadrant < 2) ? -0.5f : 0.5f;
        result.center.y += y_shift * result.size;
        return result;
    
    }
    uint get_quadrant(vec2f pos){
        return ((pos.y > center.y)) << 1 | (pos.x > center.x);
    }
    std::array<Quad,4> subdivide() const {
        std::array<Quad, 4> quads;
        for (int i = 0; i < 4; ++i) {
            quads[i] = into_quads(i);
        }
        return quads;
    }
};

class Quadtree{
private:

    float theta; 
    float th_sq;
    
    struct Node{
        
        Node(uint next_, Quad q) : next(next_), quad(q) {};
        float mass = 0.f;
        vec2f center_of_mass{0.0,0.0};
        uint children = 0;
        uint next;
        bool is_leaf() const {return children == 0;}
        Quad quad;
    };

    uint subdivide(uint node);
    vec2f calc_force(Circle_Object& object, uint node);

    float get_dist_squared(const vec2f& a, const vec2f& b){
        float x_sq = (b.x - a.x) * (b.x - a.x);
        float y_sq = (b.y - a.y) * (b.y - a.y);
        return  x_sq + y_sq;
    }
    bool should_approximate(Circle_Object& object, uint node){
        return nodes[node].quad.size * nodes[node].quad.size / get_dist_squared(object.pos,nodes[node].center_of_mass) < this->theta * this->theta;
    }
    std::vector<uint> parents;
    std::vector<Node> nodes;
public:
    Quadtree(const float theta_) : theta(theta_){ th_sq = theta * theta;};
    Quadtree(const Quadtree* other) = delete; // dont need to copy quadtrees
    ~Quadtree() = default;
    void insert_body(Circle_Object& object);
    void apply_force(Circle_Object& object);
    void update_mass_centers();
    void reset(Quad quad);
};
#pragma once
#include <memory>
#include <vector>
#include <cmath>
#include "../Simple_API/defines.hpp"
#include "../Circle_Object.hpp"
class Quadtree{
private:
    enum Quadrant{
        NW = 0,
        NE = 1,
        SW = 2,
        SE = 3,
    };

    const float theta = 0.5; 
    struct Quad{
        Quad(vec2f center_,float size_){
            center = center_;
            size = size_;
        }
        float mass = 0.f;
        vec2f center_of_mass{0.0,0.0};
        std::unique_ptr<Quad> children[4] = {nullptr,nullptr,nullptr,nullptr};
        vec2f center{0.0,0.0};
        float size;
        bool is_leaf() const {return children[0] == nullptr && children[1] == nullptr && children[2] == nullptr && children[3] == nullptr;}
    };
    inline Quadrant get_quadrant(std::unique_ptr<Quad>& quad,vec2f& body_pos){
        if (body_pos.y >= quad->center.y ){ // North
            return body_pos.x >= quad->center.x ? Quadrant::NE : Quadrant::NW;
        }else{ // South     
            return body_pos.x >=  quad->center.x ? Quadrant::SE : Quadrant::SW;
        }
    }
    void subdivide(std::unique_ptr<Quad>& quad);
    void recursive_insert(std::unique_ptr<Quad>& quad, Circle_Object& object);
    void print_quad_recursive(const std::unique_ptr<Quad>& quad, int level) const;
    void apply_force_recursively(Circle_Object& object, std::unique_ptr<Quad>& quad);
    vec2f calc_force(Circle_Object& object, std::unique_ptr<Quad>& quad);

    float get_dist_squared(const vec2f& a, const vec2f& b){
        float x_sq = (b.x - a.x) * (b.x - a.x);
        float y_sq = (b.y - a.y) * (b.y - a.y);
        return  x_sq + y_sq;
    }
    bool should_approximate(Circle_Object& object, std::unique_ptr<Quad>& quad){
        return quad->size * quad->size / get_dist_squared(object.pos,quad->center_of_mass) < this->theta * this->theta;
    }
public:
    void print_tree() const;
    std::unique_ptr<Quad> root = nullptr;
    Quadtree() = default;
    Quadtree(const Quadtree* other) = delete; // dont need to copy quadtrees
    ~Quadtree() = default;
    void insert_body(Circle_Object& object);
    void set_size(std::vector<Circle_Object>& objects);
    void apply_force(Circle_Object& object);
    void set_numeric_size(vec2f center, float size);
};
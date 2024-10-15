#include "Quadtree.hpp"
#include <iostream>
void Quadtree::insert_body(Circle_Object& object)
{
    if (!root) {
        std::vector<Circle_Object> objects = {object};
        set_size(objects);
    }
    
    recursive_insert(root,object);
}
vec2f Quadtree::calc_force(Circle_Object &object, std::unique_ptr<Quad> &quad)
{
    vec2f force = {0.0f,0.0f};
    vec2f r_vector = quad->center_of_mass - object.pos;

    float distance = sqrt(r_vector.x * r_vector.x + r_vector.y * r_vector.y);

    if (distance > 0.0){
        float force_mag = GRAVITATIONAL_CONSTANT * object.mass * quad->mass / (distance * distance);
        const float max_force = 0.01f;
        if (force_mag > max_force){
            force_mag = max_force;
        }
        force = (r_vector / distance) * force_mag;

    }
    return force;
}
void Quadtree::recursive_insert(std::unique_ptr<Quad> &quad, Circle_Object &object)
{   
    // empty quad -> set 
    if (quad->mass == 0.0f){
        quad->mass = object.mass;
        quad->center_of_mass = object.pos;
        return;
    }
    // has no children, but has data
    if(quad->is_leaf()){
        // if their positions are equal, just merge them
        if (!(quad->center_of_mass == object.pos)){
            subdivide(quad);
            Quadrant q = get_quadrant(quad,quad->center_of_mass); // Center of mass is the pos of the particle, because we have no leaves
            Circle_Object old_obj = {quad->center_of_mass.x,quad->center_of_mass.y,quad->mass};

            recursive_insert(quad->children[q],old_obj);

            Quadrant q_new = get_quadrant(quad,object.pos);
            recursive_insert(quad->children[q_new],object);
        }
    }
    // has children
    else{
        Quadrant q = get_quadrant(quad,object.pos);
        recursive_insert(quad->children[q],object);
    }
    
    float total_mass = quad->mass + object.mass;
    quad->center_of_mass = (quad->center_of_mass * quad->mass + object.pos * object.mass) / total_mass;
    quad->mass = total_mass;
}

void Quadtree::set_size(std::vector<Circle_Object> &objects)
{
    if (objects.empty())
        return;

    if (!root){
        float min_x = objects[0].pos.x;
        float max_x = objects[0].pos.x;
        float min_y = objects[0].pos.y;
        float max_y = objects[0].pos.y;

        for(const auto obj : objects){
            if (obj.pos.x < min_x) min_x = obj.pos.x;
            if (obj.pos.x > max_x) max_x = obj.pos.x;
            if (obj.pos.y < min_y) min_y = obj.pos.y;
            if (obj.pos.y > max_y) max_y = obj.pos.y;
        }

        float size = std::max(max_x - min_x, max_y - min_y);

        vec2f center{min_x + (size / 2),min_y + (size / 2)};

        root = std::make_unique<Quad>(center,size);

        //std::cout << "Root Quad has Size: " <<size << " with center: (" <<  center.x << "," << center.y << ")" << std::endl;
        //std::cout << "The total square goes from (" << center.x - size / 2 << "," << center.y - size / 2 << ") until (" << center.x + size / 2 << "," << center.y + size / 2 <<  ")" << std::endl;
    }
}

void Quadtree::set_numeric_size(vec2f center, float size)
{
    root = std::make_unique<Quad>(center,size);
}

void Quadtree::subdivide(std::unique_ptr<Quad> &quad)
{
    const float half_size = quad->size / 2.f;
    const float quater_size = half_size / 2.f;
    quad->children[NW] = std::make_unique<Quad>(vec2f(quad->center.x - quater_size, quad->center.y + quater_size),half_size);    
    quad->children[NE] = std::make_unique<Quad>(vec2f(quad->center.x + quater_size, quad->center.y + quater_size),half_size);    
    quad->children[SW] = std::make_unique<Quad>(vec2f(quad->center.x - quater_size, quad->center.y - quater_size),half_size);    
    quad->children[SE] = std::make_unique<Quad>(vec2f(quad->center.x + quater_size, quad->center.y - quater_size),half_size);    
}

void Quadtree::apply_force(Circle_Object &object)
{
    if (!root){
        object.force = vec2f{0.0f,0.0f};
        return;
    }
    apply_force_recursively(object,root);
}

void Quadtree::apply_force_recursively(Circle_Object &object, std::unique_ptr<Quad> &quad)
{   
    if (quad->center_of_mass == object.pos){
        return;
    }
    if (quad->is_leaf() || should_approximate(object,quad)){
        object.force += calc_force(object,quad);
    }else{
        for (auto& child : quad->children) {
            if (child) {
                apply_force_recursively(object, child);
            }
        }
    }
}

void Quadtree::print_tree() const {
    if (!root) {
        std::cout << "The Quadtree is empty." << std::endl;
        return;
    }

    std::cout << "Printing Quadtree:" << std::endl;
    print_quad_recursive(root, 0);
}

void Quadtree::print_quad_recursive(const std::unique_ptr<Quad>& quad, int level) const {
    if (!quad) return;

    // Indent based on the level of the tree for easier reading
    std::string indent(level * 2, ' ');

    // Print details of the current quad
    std::cout << indent << "Quad - Center: (" << quad->center.x << ", " << quad->center.y << "), "
              << "Size: " << quad->size << ", "
              << "Mass: " << quad->mass << ", "
              << "Center of Mass: (" << quad->center_of_mass.x << ", " << quad->center_of_mass.y << ")" << std::endl;

    // Check if the current quad has children, and recursively print them
    if (quad->children[NW]) {
        std::cout << indent << "NW Child:" << std::endl;
        print_quad_recursive(quad->children[NW], level + 1);
    }
    if (quad->children[NE]) {
        std::cout << indent << "NE Child:" << std::endl;
        print_quad_recursive(quad->children[NE], level + 1);
    }
    if (quad->children[SW]) {
        std::cout << indent << "SW Child:" << std::endl;
        print_quad_recursive(quad->children[SW], level + 1);
    }
    if (quad->children[SE]) {
        std::cout << indent << "SE Child:" << std::endl;
        print_quad_recursive(quad->children[SE], level + 1);
    }
}
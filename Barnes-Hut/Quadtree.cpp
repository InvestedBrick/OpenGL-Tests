#include "Quadtree.hpp"
#include <iostream>
void Quadtree::insert_body(Circle_Object& object)
{
    if (!root) {
        std::vector<Circle_Object> objects = {object};
        set_size(objects);
    }
    
    std::cout << "Inserting new body... " << std::endl;
    recursive_insert(root,object);
}

void Quadtree::recursive_insert(std::unique_ptr<Quad> &quad, Circle_Object &object)
{   
    // empty quad -> set 
    if (quad->mass == 0.0f){
        quad->mass = object.mass;
        quad->center_of_mass = object.pos;
        std::cout << "Found free node to insert" << std::endl;
        return;
    }
    // has no children, but has data
    if(quad->is_leaf()){
        // if their positiions are equal, just merge them
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

        std::cout << "Root Quad has Size: " <<size << " with center: (" <<  center.x << "," << center.y << ")" << std::endl;
        std::cout << "The total square goes from (" << center.x - size / 2 << "," << center.y - size / 2 << ") until (" << center.x + size / 2 << "," << center.y + size / 2 <<  ")" << std::endl;
    }
}

void Quadtree::subdivide(std::unique_ptr<Quad> &quad)
{
    const float half_size = quad->size / 2.f;
    const float quater_size = half_size / 2.f;
    std::cout << "Subdividing quad..." << std::endl;
    quad->children[NW] = std::make_unique<Quad>(vec2f(quad->center.x - quater_size, quad->center.y + quater_size),half_size);    
    quad->children[NE] = std::make_unique<Quad>(vec2f(quad->center.x + quater_size, quad->center.y + quater_size),half_size);    
    quad->children[SW] = std::make_unique<Quad>(vec2f(quad->center.x - quater_size, quad->center.y - quater_size),half_size);    
    quad->children[SE] = std::make_unique<Quad>(vec2f(quad->center.x + quater_size, quad->center.y - quater_size),half_size);    
}

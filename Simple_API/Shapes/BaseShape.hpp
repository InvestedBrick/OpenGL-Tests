#pragma once
#include "../defines.hpp"
#include <memory>
#include <cassert>
#include <vector>
class BaseShape{
protected:
    std::vector<float> positions;
    std::vector<uint> indices;
    float origin[2];
public:
    uint direction;
    BaseShape(const float x, const float y){
        origin[0] = x;
        origin[1] = y;
    }
    virtual ~BaseShape() = default;

    virtual void calc_positions() = 0;
    virtual void init_positions() = 0;
    virtual void init_indices() = 0;
    virtual const uint get_positions_size() const = 0;
    virtual void move(const float x, const float y) = 0;
    virtual const std::vector<float>& get_positions() const = 0;
    virtual const std::vector<uint>& get_indices() const = 0;
};
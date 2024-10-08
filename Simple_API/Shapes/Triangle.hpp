#pragma once
#include "BaseShape.hpp"
#include "../defines.hpp"
class Triangle : public BaseShape{
protected:
    float m_scale;
    const int positions_size = 6 * sizeof(float);
public:
    Triangle(const float x, const float y,const float scale = 1.f) : BaseShape(x, y), m_scale(scale) {
        init_positions();
        init_indices();
        calc_positions();
    }
    Triangle(const float f1_0,const float f1_1,const float f2_0,const float f2_1,const float f3_0,const float f3_1) : BaseShape(f1_0,f1_1){
        init_positions();
        init_indices();
        
        positions[0] = f1_0;
        positions[1] = f1_1;
        positions[2] = f2_0;
        positions[3] = f2_1;
        positions[4] = f3_0;
        positions[5] = f3_1;


    }
    void move(const float x, const float y) override{
        for (uint i = 0; i < 6;i++){
            if (i % 2 == 0){ // x coordinate
                positions[i] += x;
            }else{ // y
                positions[i] += y;
            }
        }
    }
    const uint get_positions_size()const override{
        return positions_size;
    }
    void calc_positions() override{
        

        positions[0] = origin[0];
        positions[1] = origin[1];
        positions[2] = origin[0];
        positions[3] = origin[1] - (1.f * m_scale);
        positions[4] = origin[0] + (1.f * m_scale);
        positions[5] = origin[1] - (1.f * m_scale);


    }

    const std::vector<float>& get_positions() const override{
        return positions;
    }
    const std::vector<uint>& get_indices() const override{
        return indices;
    }
    void init_positions() override{
        positions.resize(6);
    }

    void init_indices() override{
        indices.resize(3);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;

    }
};
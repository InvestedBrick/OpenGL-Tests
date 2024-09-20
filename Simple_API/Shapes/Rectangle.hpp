#pragma once
#include "../defines.hpp"
#include "BaseShape.hpp"
class Rectangle : public BaseShape{
protected:
    float m_scale;
    const int positions_size = 8 * sizeof(float);
public:    
    Rectangle(const float x, const float y,const float scale = 1.f) : BaseShape(x, y), m_scale(scale) {
        init_positions();
        init_indices();
        calc_positions();
    }
    Rectangle(const float f1_0,const float f1_1,const float f2_0,const float f2_1,const float f3_0,const float f3_1, const float f4_0,const float f4_1) : BaseShape(f1_0,f1_1){
        init_positions();
        init_indices();
        
        positions[0] = f1_0;
        positions[1] = f1_1;
        positions[2] = f2_0;
        positions[3] = f2_1;
        positions[4] = f3_0;
        positions[5] = f3_1;
        positions[6] = f4_0; 
        positions[6] = f4_1; 



    }
    const float* get_positions() const override{
        return positions.get();
    }
    const uint* get_indices() const override{
        return indices.get();
    }
    void init_positions() override{
        positions = std::make_unique<float[]>(8);
    }
    const uint get_positions_size()const override{return positions_size;}
    void calc_positions() override {
        positions[0] = origin[0];
        positions[1] = origin[1];

        positions[2] = origin[0];
        positions[3] = origin[1] - (1.f * m_scale);

        positions[4] = origin[0] + (1.f * m_scale);
        positions[5] = origin[1];

        positions[6] = positions[4];
        positions[7] = positions[5] - (1.f * m_scale);

    }

    void move(const float x, const float y) override{
    
        
        for (uint i = 0; i < 8;i++){
            if (i % 2 == 0){ // x coordinate
                positions[i] += x;
            }else{ // y
                positions[i] += y;
            }
        }


    }
    void init_indices() override{
        indices = std::make_unique<uint[]>(6);
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 2;
        indices[4] = 3;
        indices[5] = 1;

    }


};
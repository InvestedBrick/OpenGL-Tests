#pragma once
#include "BaseShape.hpp"
#include "../defines.hpp"
#include <cmath>

#define TWO_PI 6.2831853
class Circle : public BaseShape{
protected:

    float m_radius;
    uint m_segments;
    uint positions_size;
public:
    
    Circle(const float x, const float y,const uint segments,const float radius = 1.f) : BaseShape(x, y), m_radius(radius) , m_segments(segments) {
        init_positions();
        init_indices();
        calc_positions();
        positions_size = (m_segments + 2) * 2 * sizeof(float);
    }
    void move(const float x, const float y) override{
        for (uint i = 0; i < (m_segments + 2) * 2; i += 2) {
            positions[i] += x;     
            positions[i + 1] += y; 
        }
    }
    const uint get_positions_size()const override{
        return positions_size;
    }
    const uint get_indices_size() const{
        return m_segments + 2;
    }
    void calc_positions() override{
        uint idx = 2;
        positions[0] = origin[0];
        positions[1] = origin[1];
        

        float theta;
        for(int i = 0; i <= m_segments;++i){
            theta = (TWO_PI * float(i)) / float(m_segments);
            float x = m_radius * cosf(theta);
            float y = m_radius * sinf(theta);
            
            positions[idx++] = origin[0] + x;
            positions[idx++] = origin[1] + y;

        }
    }

    const float* get_positions() const override{
        return positions.get();
    }
    const uint* get_indices() const override{
        return indices.get();
    }
    void init_positions() override{
        positions = std::make_unique<float[]>((m_segments + 2) * 2);
    }

    void init_indices() override{
        indices = std::make_unique<uint[]>(m_segments + 2);
        indices[0] = 0;
        for(int i = 1; i <= m_segments; i++){
            indices[i] = i;
        }
        indices[m_segments + 1] = 1;
    }
};
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
    Circle(const float x, const float y, const uint segments, const float radius = 1.f)
        : BaseShape(x, y), m_radius(radius), m_segments(segments) {
        assert (m_segments > 0 && "Number of segments must be >= 1");
        init_positions();
        init_indices();
        calc_positions();
        positions_size = (m_segments + 1) * 2 * sizeof(float);
    }

    void move(const float x, const float y) override {
        for (uint i = 0; i < (m_segments + 1) * 2; i += 2) {
            positions[i] += x;
            positions[i + 1] += y;
        }
    }

    void set_pos(float x, float y){
        origin[0] = x;
        origin[1] = y;
        calc_positions();
    }

    const uint get_positions_size() const override {
        return positions_size;
    }

    const uint get_indices_size() const {
        return m_segments * 3;
    }
    float get_radius() const{return m_radius;};
    void calc_positions() override {
        positions[0] = origin[0];
        positions[1] = origin[1];

        uint idx = 2;
        float theta;
        for (int i = 0; i < m_segments; ++i) {
            theta = (TWO_PI * float(i)) / float(m_segments);
            float x = m_radius * cosf(theta);
            float y = m_radius * sinf(theta);

            positions[idx++] = origin[0] + x;
            positions[idx++] = origin[1] + y;
        }
    }

    const std::vector<float>& get_positions() const override {
        return positions;
    }

    const std::vector<uint>& get_indices() const override {
        return indices;
    }

    void init_positions() override {
        positions.resize((m_segments + 1) * 2);
    }

    void init_indices() override {
        indices.resize(m_segments * 3);
        for (uint i = 0; i < m_segments; ++i) {
            indices[i * 3] = 0;
            indices[i * 3 + 1] = i + 1;
            indices[i * 3 + 2] = (i + 1) % m_segments + 1;
        }

    }
};
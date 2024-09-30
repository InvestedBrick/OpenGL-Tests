#pragma once
#include "defines.hpp"

class ShaderStorageBuffer{
private:
    unsigned int m_Renderer_ID;
public:
    ShaderStorageBuffer(const void* data, uint size);
    ~ShaderStorageBuffer();

    void bind() const ;
    void unbind() const ;
    void bind_to(uint binding);
    void update_data(const void* data, uint size);
};
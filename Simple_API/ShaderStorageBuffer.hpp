#pragma once
#include "defines.hpp"

class ShaderStorageBuffer{
private:
    unsigned int m_Renderer_ID;
public:
    ShaderStorageBuffer(const void* data, unsigned int size);
    ~ShaderStorageBuffer();

    void bind() const ;
    void bind_to(uint binding);
    void unbind() const ;
};
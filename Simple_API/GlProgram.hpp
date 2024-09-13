#pragma once
#include <vector>
#include <string>
#include <unordered_map>

#include "defines.hpp"

class GlProgram{
private:
    uint m_Renderer_ID;
    std::vector<uint> m_shaders;
    std::unordered_map<std::string, int> m_uniform_cache;

public:

    GlProgram(std::vector<uint> shaders);
    ~GlProgram();

    void bind() const;
    void unbind() const;
    void delete_shaders();

    void set_uniform_4f(const std::string& name, float f0,float f1, float f2, float f3);

    uint get_Renderer_ID() const {return m_Renderer_ID;}

private:

    int get_uniform_location(const std::string& name);
};
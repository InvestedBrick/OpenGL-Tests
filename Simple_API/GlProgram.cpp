#include "GlProgram.hpp"
#include <iostream>
GlProgram::GlProgram(std::vector<uint> shaders)
    : m_shaders(shaders)
{
    m_Renderer_ID = glCreateProgram();

    for (const uint shader: shaders){
        glCall(glAttachShader(m_Renderer_ID,shader));
    }
    glCall(glLinkProgram(m_Renderer_ID));
    glCall(glValidateProgram(m_Renderer_ID));

}

GlProgram::~GlProgram()
{
    glCall(glDeleteProgram(m_Renderer_ID));
}

void GlProgram::bind() const
{
    glCall(glUseProgram(m_Renderer_ID));
}

void GlProgram::unbind() const
{
    glCall(glUseProgram(0));
}

void GlProgram::delete_shaders()
{
    for (const uint shader: m_shaders){
        glCall(glDeleteShader(shader))
    }
}

void GlProgram::set_uniform_4f(const std::string &name, float f0, float f1, float f2, float f3)
{
    glCall(glUniform4f(get_uniform_location(name),f0,f1,f2,f3));
}
void GlProgram::set_uniform_mat4f(const std::string &name, uint count, bool transpose, const float *data)
{
    glCall(glUniformMatrix4fv(get_uniform_location(name),count,transpose,data));
}
void GlProgram::set_uniform_1ui(const std::string &name, int i)
{
    glCall(glUniform1ui(get_uniform_location(name),i));
}
int GlProgram::get_uniform_location(const std::string &name)
{   
    if(m_uniform_cache.find(name) != m_uniform_cache.end()){
        return m_uniform_cache[name];
    }
    glCall(int location = glGetUniformLocation(m_Renderer_ID,name.c_str()));

    if (location == -1){
        std::cerr << "[WARNING]: Uniform '" << name << "' does not exist!" << std::endl;
    }
    m_uniform_cache[name] = location;
    return location;
}

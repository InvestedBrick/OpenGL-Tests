#include "Shader.hpp"
#include "Renderer.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string &filepath, uint type)
    : m_filepath(filepath), m_Shader_ID(0), m_type(type)
{

    const std::string src = load_shader();
    m_Shader_ID = compile_shader(src);
}

Shader::~Shader()
{
    glCall(glDeleteShader(m_Shader_ID));
}

uint Shader::compile_shader(const std::string& src)
{
    uint id = glCreateShader(m_type);
    const char* source = src.c_str();

    // length is null because source is null-terminated
    glCall(glShaderSource(id, 1, &source, nullptr));
    glCall(glCompileShader(id));

    int result;
    // Check for compile errors
    glCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE) {
        int len;
        glCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len));
        char* msg = (char*)alloca(len * sizeof(char));
        glCall(glGetShaderInfoLog(id, len, &len, msg));

        std::cerr << "Failed to compile "<<  m_filepath << std::endl;
        std::cerr << msg << std::endl;
        glCall(glDeleteShader(id));
        return 0;
    }
    return id;
}

std::string Shader::load_shader()
{
    std::ifstream file(m_filepath);
    if (!file.is_open() || !file.good()) {
        std::cerr << "Failed to open " << m_filepath << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream code;
    code << file.rdbuf();
    return code.str();
}


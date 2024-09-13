#pragma once
#include <string>

class Shader{
private:
    uint m_Shader_ID;
    uint m_type;
    std::string m_filepath;
public:
    Shader(const std::string& filepath,uint type);
    ~Shader();

    inline uint get_shader_ID() const { return m_Shader_ID;}


private:
    uint compile_shader(const std::string& src);
    std::string load_shader();
};
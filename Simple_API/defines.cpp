#include "defines.hpp"
#include <iostream>

void GlClearError(){
    while(!(glGetError() == GL_NO_ERROR));
}
bool GlLogErrors(){
    while(GLenum err = glGetError()){
        std::cout << "[OpenGL Error]: " << "0x"<<std::hex <<err << std::endl;
        return false;
    }
    return true;
}
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <cassert>

#include "Renderer.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "VertexArray.hpp"
#include "VertexArray.hpp"

static std::string LoadShader(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open() || !file.good()) {
        std::cerr << "Failed to open " << filepath << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream code;
    code << file.rdbuf();
    return code.str();
}

static uint compileShader(const std::string& src, uint type) {
    uint id = glCreateShader(type);
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

        std::cerr << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << std::endl;
        std::cerr << msg << std::endl;
        glCall(glDeleteShader(id));
        return 0;
    }
    return id;
}

static uint CreateShader(const std::string& vertex_src, const std::string& fragment_src) {
    uint prog = glCreateProgram();
    uint vs = compileShader(vertex_src, GL_VERTEX_SHADER);
    uint fs = compileShader(fragment_src, GL_FRAGMENT_SHADER);

    // Link the shaders together
    glCall(glAttachShader(prog, vs));
    glCall(glAttachShader(prog, fs));
    glCall(glLinkProgram(prog));
    glCall(glValidateProgram(prog));

    // Delete shaders because they are now linked in the program
    glCall(glDeleteShader(vs));
    glCall(glDeleteShader(fs));

    return prog;
}

int main(void) {

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        std::cerr << "glfw Init Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello Rectangle", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Window initialization failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Glew initialization Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    {
    float positions[] = {
        -0.5f, -0.5f, //0
        0.5f,  -0.5f, //1
        0.5f, 0.5f,   //2
        -0.5f, 0.5f,  //3
                

    };
        uint indices[] = {
            0,1,2,
            2,3,0
        };
    
        VertexArray vao;
        VertexBuffer vb(positions,8 * sizeof(float));
        VertexBufferLayout vbl;

        vbl.push(GL_FLOAT,2);
        vao.add_buffer(vb,vbl);
    
        IndexBuffer ibo(indices,6);
    
        // Use the LoadShader function to read the shader files
        std::string vertex = LoadShader("./shaders/vertex.shader");
        std::string fragment = LoadShader("./shaders/fragment.shader");
        float r = 0.0f;
        float inc_r= 0.005f;
        uint shader = CreateShader(vertex, fragment);
        glCall(glUseProgram(shader));
        /* Loop until the user closes the window */
    
        /* Unbind eveything */
        glCall(glBindVertexArray(0));
        glCall(glUseProgram(0));
        glCall(glBindBuffer(GL_ARRAY_BUFFER,0));
        glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0));
        while (!glfwWindowShouldClose(window)) {
            /* Render here */
            glCall(glClear(GL_COLOR_BUFFER_BIT)); 
            glCall(glUseProgram(shader));
            glCall(glUniform4f(glGetUniformLocation(shader,"u_color"),r, 0.f, 0.7f,1.0f))
    
            vao.bind();
            ibo.bind();
            glCall(glDrawElements(GL_TRIANGLES, 6,GL_UNSIGNED_INT, nullptr));
    
            if(r > 1.0f){
                inc_r = -0.01f;
            }else if(r < 0.0f){
                inc_r = 0.01f;
            }
            r+= inc_r;
    
            /* Swap front and back buffers */
            glCall(glfwSwapBuffers(window));
    
            /* Poll for and process events */
            glCall(glfwPollEvents());
        }

        glDeleteProgram(shader);
    }
    glfwTerminate();
    return 0;
}

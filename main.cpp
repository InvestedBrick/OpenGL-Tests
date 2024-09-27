#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <random>

#include "Simple_API/defines.hpp"
#include "Simple_API/VertexBuffer.hpp"
#include "Simple_API/IndexBuffer.hpp"
#include "Simple_API/VertexArray.hpp"
#include "Simple_API/Shader.hpp"
#include "Simple_API/GlProgram.hpp"
#include "Simple_API/Renderer.hpp"

#include "Simple_API/Shapes/Triangle.hpp"
#include "Simple_API/Shapes/Rectangle.hpp"
#include "Simple_API/Shapes/Circle.hpp"

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1080

#define VSYNC 1

void unbind_all() {
    /* Unbind everything */
    glCall(glBindVertexArray(0));
    glCall(glUseProgram(0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

int main(void) {

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(-0.95f,0.95f);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        std::cerr << "glfw Init Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "Hello OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        std::cerr << "Window initialization failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(VSYNC);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Glew initialization Failed" << std::endl;
        exit(EXIT_FAILURE);
    }


    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    
    #define N_ITEMS 4
    #define SCALE 0.5f
    
    #define N_CIRCLES 300000
    #define CIRCLE_SEGMENTS 10
    //Rectangle squares[N_ITEMS] = {{-1.f,1.f,SCALE}
    //                             ,{1.f - SCALE,1.f,SCALE}
    //                             ,{1.f - SCALE,-(1.f- SCALE),SCALE}
    //                             ,{-1.f,-(1.f - SCALE),SCALE}};
    std::vector<Circle> circs; circs.reserve(N_CIRCLES);

    for (size_t i = 0; i < N_CIRCLES; i++) {
        circs.emplace_back(dist(rng), dist(rng), CIRCLE_SEGMENTS, 0.002f);
    }

    const uint circ_pos_size = N_CIRCLES * ((CIRCLE_SEGMENTS + 1) * 2);
    const uint circ_idx_size = N_CIRCLES * ((CIRCLE_SEGMENTS * 3));

    std::vector<float> circ_positions; circ_positions.reserve(circ_pos_size);
    std::vector<uint> circ_indices; circ_indices.reserve(circ_idx_size);

    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<float>& positions = circs[i].get_positions();
        circ_positions.insert(circ_positions.end(),positions.begin(),positions.end());
    }
    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<uint>& indices = circs[i].get_indices();
        for (int j = 0; j < (CIRCLE_SEGMENTS * 3); j++) {
            circ_indices[(i * (CIRCLE_SEGMENTS * 3)) + j] = indices[j] + (i * ((CIRCLE_SEGMENTS + 1) ));
        }
    }


    VertexBuffer circ_vb(circ_positions.data(),circ_pos_size * sizeof(float));
    IndexBuffer circ_ibo(circ_indices.data(),circ_idx_size);
    
    VertexArray circ_vao;
    VertexBufferLayout circ_vbl;
    circ_vbl.push(GL_FLOAT,2);
    circ_vao.add_buffer(circ_vb,circ_vbl);


//
//    const float* positions[N_ITEMS];
//    const uint* indices[N_ITEMS];
//    for(uint i = 0; i < N_ITEMS; i++){
//        positions[i] = squares[i].get_positions();
//        indices[i] = squares[i].get_indices();
//        squares[i].direction = i;
//    }
//    const uint size = squares[0].get_positions_size();
//    const uint ib_size = 6;
//
//    VertexArray vaos[N_ITEMS];
//    VertexBuffer vbs[N_ITEMS] = {{positions[0],size},{positions[1],size},{positions[2],size},{positions[3],size}};
//    IndexBuffer ibos[N_ITEMS] = {{indices[0],ib_size},{indices[1],ib_size},{indices[2],ib_size},{indices[3],ib_size}};
//
//    VertexBufferLayout vbls[N_ITEMS];
//    for(uint i = 0; i < N_ITEMS; i++){
//        vbls[i].push(GL_FLOAT,2);
//        vaos[i].add_buffer(vbs[i],vbls[i]);
//    }
    // Initialize shaders and program
    GlProgram prog({Shader("./shaders/vertex.shader", GL_VERTEX_SHADER).get_shader_ID(),
                    Shader("./shaders/fragment.shader", GL_FRAGMENT_SHADER).get_shader_ID()});
    prog.bind();
    prog.delete_shaders();
    unbind_all();

    Renderer renderer;
    const float to_move = 0.01f;
    float last_time = 0.f;
    int frame_count = 0;
    const int update_interval = 30;  

    while (!glfwWindowShouldClose(window)) {
        float current_time = glfwGetTime();
        float dt = current_time - last_time;
        last_time = current_time;

        frame_count++; 
        if (frame_count >= update_interval) {
            
            std::string title = "Hello OpenGL at: " + std::to_string((1/dt)) + " FPS"; 
            glfwSetWindowTitle(window, title.c_str());
            frame_count = 0;  
        }

        /* Render here */
        renderer.clear();
        // Bind the program and set the uniform color
        prog.bind();
        prog.set_uniform_4f("u_color", 1.f, 0.f, 0.0f, 1.0f);
        /*
        for(uint i = 0; i < N_ITEMS; i++){
            const float* move_pos = squares[i].get_positions();
            switch (squares[i].direction) {
                case 0:  // moving right
                    squares[i].move(to_move, 0.f);
                    if (move_pos[0] >= 1.f - SCALE) squares[i].direction = 1;  // Switch to moving down
                    break;
                case 1:  // moving down
                    squares[i].move(0.f, -to_move);
                    if (move_pos[1] <= -(1.f - SCALE)) squares[i].direction = 2;  // Switch to moving left
                    break;
                case 2:  // moving left
                    squares[i].move(-to_move, 0.f);
                    if (move_pos[0] <= -1.f) squares[i].direction = 3;  // Switch to moving up
                    break;
                case 3:  // moving up
                    squares[i].move(0.f, to_move);
                    if (move_pos[1] >= 1.f ) squares[i].direction = 0;  // Switch to moving right
                    break;
            }
            vbs[i].bind();
            glCall(glBufferSubData(GL_ARRAY_BUFFER, 0, squares[i].get_positions_size(), squares[i].get_positions()));
            renderer.draw(vaos[i],ibos[i],prog);
            vbs[i].unbind();
        }
        */
        renderer.draw(circ_vao,circ_ibo,prog,GL_TRIANGLES);

        
        /* Swap front and back buffers */
        glCall(glfwSwapBuffers(window));
        /* Poll for and process events */
        glCall(glfwPollEvents());
    }
    glfwTerminate();
    return 0;
}


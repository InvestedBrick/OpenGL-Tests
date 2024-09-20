#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include "Simple_API/defines.hpp"
#include "Simple_API/VertexBuffer.hpp"
#include "Simple_API/IndexBuffer.hpp"
#include "Simple_API/VertexArray.hpp"
#include "Simple_API/Shader.hpp"
#include "Simple_API/GlProgram.hpp"
#include "Simple_API/Renderer.hpp"

#include "Simple_API/Shapes/Triangle.hpp"
#include "Simple_API/Shapes/Rectangle.hpp"
#define PI 3.14159265358979323846

#define WINDOW_HEIGHT 640
#define WINDOW_WIDTH 640

// Generates vertices for a circle
void generateCircleVertices(float* positions, unsigned int* indices, int numSegments, float radius) {
    // First vertex is the center of the circle
    positions[0] = 0.0f;
    positions[1] = 0.0f;

    // Generate points around the circumference
    for (int i = 1; i <= numSegments; ++i) {
        float angle = 2.0f * PI * (i - 1) / numSegments;
        positions[i * 2] = radius * cos(angle);  // x
        positions[i * 2 + 1] = radius * sin(angle);  // y
    }

    // Generate indices for the triangle fan
    for (int i = 0; i < numSegments; ++i) {
        indices[i * 3] = 0;  // Center point
        indices[i * 3 + 1] = i + 1;  // Current point on circumference
        indices[i * 3 + 2] = (i + 1) % numSegments + 1;  // Next point on circumference
    }
}

void unbind_all() {
    /* Unbind everything */
    glCall(glBindVertexArray(0));
    glCall(glUseProgram(0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

int main(void) {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
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
    glfwSwapInterval(1);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Glew initialization Failed" << std::endl;
        exit(EXIT_FAILURE);
    }


    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    {

        #define N_ITEMS 4
        #define SCALE 0.5f
        Rectangle squares[N_ITEMS] = {{-1.f,1.f,SCALE}
                                     ,{1.f - SCALE,1.f,SCALE}
                                     ,{1.f - SCALE,-(1.f- SCALE),SCALE}
                                     ,{-1.f,-(1.f - SCALE),SCALE}};


        const float* positions[N_ITEMS];
        const uint* indices[N_ITEMS];

        for(uint i = 0; i < N_ITEMS; i++){
            positions[i] = squares[i].get_positions();
            indices[i] = squares[i].get_indices();
            squares[i].direction = i;
        }

        const uint size = squares[0].get_positions_size();
        const uint ib_size = 6;

        VertexArray vaos[N_ITEMS];
        VertexBuffer vbs[N_ITEMS] = {{positions[0],size},{positions[1],size},{positions[2],size},{positions[3],size}};
        IndexBuffer ibos[N_ITEMS] = {{indices[0],ib_size},{indices[1],ib_size},{indices[2],ib_size},{indices[3],ib_size}};
        VertexBufferLayout vbls[N_ITEMS];

        for(uint i = 0; i < N_ITEMS; i++){
            vbls[i].push(GL_FLOAT,2);
            vaos[i].add_buffer(vbs[i],vbls[i]);
        }

        // Set up rendering parameters (color cycling)
        float r = 0.0f;
        float g = 0.8f;
        float inc_r, inc_g = 0.01f;

        // Initialize shaders and program
        GlProgram prog({Shader("./shaders/vertex.shader", GL_VERTEX_SHADER).get_shader_ID(),
                        Shader("./shaders/fragment.shader", GL_FRAGMENT_SHADER).get_shader_ID()});
        prog.bind();
        prog.delete_shaders();

        // Unbind everything to ensure clean state
        unbind_all();

        Renderer renderer;
        const float to_move = 0.01f;
        while (!glfwWindowShouldClose(window)) {
            /* Render here */
            renderer.clear();

            // Bind the program and set the uniform color
            prog.bind();
            prog.set_uniform_4f("u_color", r, g, 0.7f, 1.0f);

            for(uint i = 0; i < 4; i++){

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

            // Update color cycling
            if (r > 1.0f) {
                inc_r = -0.01f;
            } else if (r < 0.0f) {
                inc_r = 0.01f;
            }
            r += inc_r;

            if (g > 1.0f) {
                inc_g = -0.01f;
            } else if (g < 0.0f) {
                inc_g = 0.02f;
            }
            g += inc_g;

            /* Swap front and back buffers */
            glCall(glfwSwapBuffers(window));

            /* Poll for and process events */
            glCall(glfwPollEvents());
        }
    }

    glfwTerminate();
    return 0;
}


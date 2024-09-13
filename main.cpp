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
    window = glfwCreateWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "Hello Circle", NULL, NULL);
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
        const int numSegments = 10;  // Number of segments for the circle
        const float radius = 0.4f;    // Radius of the circle

        // Allocate space for the positions and indices
        float positions[(numSegments + 1) * 2];  // (numSegments + 1) vertices, 2 components per vertex (x, y)
        unsigned int indices[numSegments * 3];   // numSegments triangles, 3 indices per triangle

        // Generate the circle vertices and indices
        generateCircleVertices(positions, indices, numSegments, radius);

        // Setup buffers and VAO
        VertexArray vao;
        VertexBuffer vb(positions, (numSegments + 1) * 2 * sizeof(float));
        IndexBuffer ibo(indices, numSegments * 3);
        VertexBufferLayout vbl;

        vbl.push(GL_FLOAT, 2);
        vao.add_buffer(vb, vbl);

        float r = 0.0f;
        float g = 0.8f;
        float inc_r, inc_g = 0.01f;

        GlProgram prog({Shader("./shaders/vertex.shader", GL_VERTEX_SHADER).get_shader_ID(),
                        Shader("./shaders/fragment.shader", GL_FRAGMENT_SHADER).get_shader_ID()});
        prog.bind();
        prog.delete_shaders();

        /* Loop until the user closes the window */
        unbind_all();

        Renderer renderer;

        while (!glfwWindowShouldClose(window)) {
            /* Render here */
            renderer.clear();
            prog.bind();
            prog.set_uniform_4f("u_color", r, g, 0.7f, 1.0f);

            renderer.draw(vao, ibo, prog);

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

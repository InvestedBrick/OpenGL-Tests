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
#define GRAVITATIONAL_CONSTANT 0.001
#define VSYNC 1

struct vec2f{
    float x,y;

    vec2f(float pos_x, float pos_y){
        x = pos_x;
        y = pos_y;
    }

    vec2f operator+(const vec2f& other) const{
        return vec2f(x + other.x,y + other.y);
    }
    vec2f& operator+=(const vec2f& other) {
        x += other.x;
        y += other.y;
        return *this; 
    }
    vec2f operator-(const vec2f& other) const{
        return vec2f(x - other.x,y - other.y);
    }
    vec2f operator*(const float scalar) const{
        return vec2f(x * scalar,y * scalar);
    }
    vec2f operator/(const float scalar) const{
        return vec2f(x / scalar,y / scalar);
    }
};



struct Circle_Object
{
    Circle_Object(const float x, const float y, const uint segments,uint vb_idx,uint m, const float radius = 1.f,const vec2f v = {0.f,0.f}) 
        : circ(x,y,segments,radius),vb_pos(vb_idx), mass(m),pos(x,y), vel(v){}
    Circle circ;
    uint vb_pos;
    uint mass;
    vec2f pos;
    vec2f vel;
};


vec2f compute_gravitational_force(const  Circle_Object& body1, const Circle_Object& body2) {
    vec2f force = {0.0f, 0.0f};
    vec2f r_vector = body2.pos - body1.pos;
    float distance = sqrt(r_vector.x * r_vector.x + r_vector.y * r_vector.y);

    if (distance > 0){
        float force_magnitude = GRAVITATIONAL_CONSTANT * body1.mass * body2.mass / (distance * distance);
        force_magnitude = force_magnitude > 0.002 ? 0.002 : force_magnitude;
        force = (r_vector / distance) * force_magnitude;
    }
    return force;
}
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
    std::uniform_real_distribution<float> dist(-1.f,1.f);
    std::uniform_real_distribution<float> mass_dist(1.0f,21.0f);

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
    
    #define N_CIRCLES 501
    #define CIRCLE_SEGMENTS 10
    //Rectangle squares[N_ITEMS] = {{-1.f,1.f,SCALE}
    //                             ,{1.f - SCALE,1.f,SCALE}
    //                             ,{1.f - SCALE,-(1.f- SCALE),SCALE}
    //                             ,{-1.f,-(1.f - SCALE),SCALE}};
    std::vector<Circle_Object> circs; circs.reserve(N_CIRCLES);

    for (size_t i = 0; i < N_CIRCLES - 1; i++) {
        float mass = mass_dist(rng);
        float pos_x = dist(rng);
        float pos_y = dist(rng);
        circs.emplace_back(pos_x,pos_y,CIRCLE_SEGMENTS,i * ((CIRCLE_SEGMENTS + 1) * 2) * sizeof(float),mass,mass * 0.0005f,vec2f{pos_y / 10,-pos_x / 5});
    }
    circs.emplace_back(0,0,CIRCLE_SEGMENTS,(N_CIRCLES - 1) *((CIRCLE_SEGMENTS + 1) * 2) * sizeof(float),1000,0.1,vec2f{0.f,0.f});

    const uint circ_pos_size = N_CIRCLES * ((CIRCLE_SEGMENTS + 1) * 2);
    const uint circ_idx_size = N_CIRCLES * ((CIRCLE_SEGMENTS * 3));

    std::vector<float> circ_positions; circ_positions.reserve(circ_pos_size);
    std::vector<uint> circ_indices; circ_indices.reserve(circ_idx_size);

    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<float>& positions = circs[i].circ.get_positions();
        circ_positions.insert(circ_positions.end(),positions.begin(),positions.end());
    }
    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<uint>& indices = circs[i].circ.get_indices();
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
        std::vector<vec2f> forces;
        for (int i = 0; i < N_CIRCLES; ++i) {
            forces.push_back({0.f,0.f});
        }

        for (int i = 0; i < N_CIRCLES; ++i) {
            for (int j = 0; j < N_CIRCLES; ++j) {
                if (i != j) {
                    forces[i] += compute_gravitational_force(circs[i], circs[j]);
                }
            }
        }
        for (int i = 0; i < N_CIRCLES; ++i) {
            circs[i].vel += (forces[i] / circs[i].mass) * dt;
            circs[i].pos += circs[i].vel * dt;
            circs[i].circ.set_pos(circs[i].pos.x,circs[i].pos.y);
            circ_vb.bind();
            glCall(glBufferSubData(GL_ARRAY_BUFFER,circs[i].vb_pos,circs[i].circ.get_positions_size(),circs[i].circ.get_positions().data()));
            circ_vb.unbind();
        }
        /* Render here */
        renderer.clear();
        // Bind the program and set the uniform color
        prog.bind();
        prog.set_uniform_4f("u_color", 1.f, 0.f, 0.0f, 1.0f);
        renderer.draw(circ_vao,circ_ibo,prog,GL_TRIANGLES);

        
        /* Swap front and back buffers */
        glCall(glfwSwapBuffers(window));
        /* Poll for and process events */
        glCall(glfwPollEvents());
    }
    glfwTerminate();
    return 0;
}


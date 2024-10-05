#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>          

#include <random>
#include <string>

#include "Simple_API/defines.hpp"
#include "Simple_API/VertexBuffer.hpp"
#include "Simple_API/IndexBuffer.hpp"
#include "Simple_API/VertexArray.hpp"
#include "../Graphics/Simple_API/ShaderStorageBuffer.hpp"
#include "Simple_API/Shader.hpp"
#include "Simple_API/GlProgram.hpp"
#include "Simple_API/Renderer.hpp"

#include "Simple_API/Shapes/Triangle.hpp"
#include "Simple_API/Shapes/Rectangle.hpp"
#include "Simple_API/Shapes/Circle.hpp"

#include "vec2.hpp"
#include "Circle_Object.hpp" // I need the struct template for quadtrees too

#define WINDOW_HEIGHT 1080
#define WINDOW_WIDTH 1080
#define GRAVITATIONAL_CONSTANT 0.01
#define MASS_TO_RADIUS 0.0001
#define MOUSE_NOTHING_POS -10
#define VSYNC 1
#define CPU_STATE 1
#define GPU_STATE 0
#define N_CIRCLES 2000
#define CIRCLE_SEGMENTS 10



struct Circle_Texture
{
    Circle_Texture(const float x, const float y,uint n_segments,const uint vb_position,const float radius = 1.f) : circ(x,y,n_segments,radius),vb_pos(vb_position){}
    Circle circ;
    uint vb_pos;
};


vec2f compute_gravitational_force(Circle_Object& body1, const Circle_Object& body2) {
    vec2f force = {0.0f, 0.0f};
    vec2f r_vector = body2.pos - body1.pos;
    body1.padding_0 = body2.pos.x;
    float distance = sqrt(r_vector.x * r_vector.x + r_vector.y * r_vector.y);
    body1.padding_0 = distance * distance;
    if (distance > 0.0){
        float force_magnitude = GRAVITATIONAL_CONSTANT * body1.mass * body2.mass / (distance * distance);
        const float max_force = 0.01f; 
        if (force_magnitude > max_force) {
            force_magnitude = max_force;
        }
        force = (r_vector / distance) * force_magnitude;
    }
    return force;
}
void unbind_all() {
    glCall(glBindVertexArray(0));
    glCall(glUseProgram(0));
    glCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    glCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
float global_mouse_x = MOUSE_NOTHING_POS;
float global_mouse_y = MOUSE_NOTHING_POS;
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
float zoomLevel = 1.0f;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = (xpos / (WINDOW_WIDTH / 2)) - 1;
        ypos = (-ypos / (WINDOW_HEIGHT / 2)) + 1;

        global_mouse_x = xpos;
        global_mouse_y = ypos;
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        global_mouse_x = MOUSE_NOTHING_POS;
        global_mouse_y = MOUSE_NOTHING_POS;
    }
}
bool W_Pressed = false;
bool A_Pressed = false;
bool S_Pressed = false;
bool D_Pressed = false;
bool Up_Pressed = false;
bool Down_Pressed = false;
bool paused = false;
void key_button_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    float cameraSpeed = 0.005f;
    
    // Update key states when keys are pressed or released
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) W_Pressed = true;
        else if (action == GLFW_RELEASE) W_Pressed = false;
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) A_Pressed = true;
        else if (action == GLFW_RELEASE) A_Pressed = false;
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) S_Pressed = true;
        else if (action == GLFW_RELEASE) S_Pressed = false;
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) D_Pressed = true;
        else if (action == GLFW_RELEASE) D_Pressed = false;
    }
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) Up_Pressed = true;
        else if (action == GLFW_RELEASE) Up_Pressed = false;
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) Down_Pressed = true;
        else if (action == GLFW_RELEASE) Down_Pressed = false;
    }
    if (key == GLFW_KEY_SPACE) {
        if (action == GLFW_PRESS && !paused) paused = true;
        else if (action == GLFW_PRESS && paused) paused = false;
    }
}
void updateCamera() {
    float cameraSpeed = 0.02f;
    
    // Move camera based on key states
    if (W_Pressed) {
        cameraPosition.y -= cameraSpeed;
    }
    if (A_Pressed) {
        cameraPosition.x += cameraSpeed;
    }
    if (S_Pressed) {
        cameraPosition.y += cameraSpeed;
    }
    if (D_Pressed) {
        cameraPosition.x -= cameraSpeed;
    }
    if (Up_Pressed) {
        zoomLevel += cameraSpeed;
    }
    if (Down_Pressed && zoomLevel > 0.1f) {
        zoomLevel -= cameraSpeed;
    }
}
int main(int argc, char* argv[]) {
    bool state;
    if (argc == 2){
        if(std::string(argv[1]) == "-cpu"){
            state = CPU_STATE;
            std::cout << "Set CPU State" << std::endl;
        }
        else if (std::string(argv[1]) == "-gpu"){
            state = GPU_STATE;
            std::cout << "Set GPU State" << std::endl;
        }
    }
    std::random_device dev;
    std::mt19937 rng(dev());
    std::normal_distribution<float> dist(0,1.f);
    std::normal_distribution<float> mass_dist(50.f,10.f);
    std::uniform_real_distribution<float> vel_dist(-0.03f, 0.03f);

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


    glfwSetMouseButtonCallback(window,mouse_button_callback);
    glfwSetKeyCallback(window,key_button_callback);
    glfwSwapInterval(VSYNC);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Glew initialization Failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    glCall(glViewport(0,0,WINDOW_WIDTH,WINDOW_HEIGHT));

    std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
    
    const uint local_size = 64; // must be same as in compute shader
    const uint work_group_size = (N_CIRCLES + local_size - 1) / local_size;
    std::vector<Circle_Object> circs; circs.reserve(N_CIRCLES);
    std::vector<Circle_Texture> circle_textures; circle_textures.reserve(N_CIRCLES);
    for (size_t i = 0; i < N_CIRCLES ; i++) {
        float mass = mass_dist(rng); 
        float pos_x = dist(rng);
        float pos_y = dist(rng);
        vec2f random_velocity = vec2f(vel_dist(rng), vel_dist(rng));
        circs.emplace_back(pos_x,pos_y,mass,vec2f{ pos_y / 5 + random_velocity.x, -pos_x / 5 + random_velocity.y}); 
        circle_textures.emplace_back(pos_x,pos_y,CIRCLE_SEGMENTS,i * ((CIRCLE_SEGMENTS + 1) * 2) * sizeof(float),mass * MASS_TO_RADIUS);
    }   
    const uint circ_pos_size = N_CIRCLES * ((CIRCLE_SEGMENTS + 1) * 2);
    const uint circ_idx_size = N_CIRCLES * ((CIRCLE_SEGMENTS * 3));

    std::vector<float> circ_positions; circ_positions.reserve(circ_pos_size);
    std::vector<uint> circ_indices; circ_indices.reserve(circ_idx_size);

    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<float>& positions = circle_textures[i].circ.get_positions();
        circ_positions.insert(circ_positions.end(),positions.begin(),positions.end());
    }
    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<uint>& indices = circle_textures[i].circ.get_indices();
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
    
    //Only works, becaouse circles are constant
    ShaderStorageBuffer ssbo(circs.data(),circs.size() * sizeof(Circle_Object));
    ssbo.bind_to(0);
    
    GlProgram prog({Shader("./shaders/vertex.shader", GL_VERTEX_SHADER).get_shader_ID(),
                    Shader("./shaders/fragment.shader", GL_FRAGMENT_SHADER).get_shader_ID()});
    prog.bind();
    prog.delete_shaders();
    
    GlProgram compute_prog({Shader("./shaders/compute.shader",GL_COMPUTE_SHADER).get_shader_ID()});
    compute_prog.bind();
    compute_prog.delete_shaders();

    

    Renderer renderer;
    float last_time = 0.f;
    int frame_count = 0;
    const int update_interval = 30;  

    glCall(glMatrixMode(GL_PROJECTION));

    paused = true;
    unbind_all();
    const float calculation_dt = 1.f / 60.f; // aim for 60 frames
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
        updateCamera();
        renderer.clear();


        /* Camera*/
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f),glm::vec3(cameraPosition.x, cameraPosition.y, 0.0f));
        view = glm::scale(view, glm::vec3(zoomLevel, zoomLevel, 1.0f));
        prog.bind();
        prog.set_uniform_mat4f("u_projection",1,GL_FALSE,glm::value_ptr(projection));
        prog.set_uniform_mat4f("u_view",1,GL_FALSE,glm::value_ptr(view));

        if (!paused){
            if (state == GPU_STATE){
                compute_prog.bind();
                ssbo.update_data(circs.data(),circs.size() * sizeof(Circle_Object));
                glCall(glDispatchCompute(work_group_size, 1, 1));  
                glCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)); 
                Circle_Object* data = (Circle_Object*)glMapBuffer(GL_SHADER_STORAGE_BUFFER,GL_READ_ONLY);
                if (data){
                    for (uint i = 0; i < N_CIRCLES;++i){
                        circs[i].vel = data[i].vel;
                    }
                    glCall(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
                }
            }
            else if (state == CPU_STATE){
                for (int i = 0; i < N_CIRCLES; ++i) {
                    for (int j = 0; j < N_CIRCLES; ++j) {
                        if (i != j) {
                            circs[i].force += compute_gravitational_force(circs[i], circs[j]);
                        }
                    }
                }
            }
            
            for (int i = 0; i < N_CIRCLES; ++i) {
                if (state != GPU_STATE){
                    circs[i].vel += (circs[i].force / circs[i].mass) * calculation_dt;
                    circs[i].force.x = 0;
                    circs[i].force.y = 0;
                }
                if(global_mouse_x != MOUSE_NOTHING_POS){
                    vec2f dist = (vec2f(global_mouse_x,global_mouse_y) - circs[i].pos);
                    float distance = sqrt(dist.x * dist.x + dist.y * dist.y);
                    if (distance < 0.2){
                        circs[i].vel -= (vec2f(global_mouse_x,global_mouse_y) - circs[i].pos) / (circs[i].mass * 0.01f) * calculation_dt;
                    }
                }
                circs[i].pos += circs[i].vel * calculation_dt;
                circle_textures[i].circ.set_pos(circs[i].pos.x,circs[i].pos.y);
                circ_vb.bind();
                glCall(glBufferSubData(GL_ARRAY_BUFFER,circle_textures[i].vb_pos,circle_textures[i].circ.get_positions_size(),circle_textures[i].circ.get_positions().data()));
                circ_vb.unbind();
            }
        }
        /* Render here */
        renderer.draw(circ_vao,circ_ibo,prog,GL_TRIANGLES);

        /* Swap front and back buffers */
        glCall(glfwSwapBuffers(window));
        /* Poll for and process events */
        glCall(glfwPollEvents());
    }

    glfwTerminate();
    return 0;
}


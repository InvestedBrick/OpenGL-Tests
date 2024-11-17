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
#include "Simple_API/ShaderStorageBuffer.hpp"
#include "Simple_API/Shader.hpp"
#include "Simple_API/GlProgram.hpp"
#include "Simple_API/Renderer.hpp"

#include "Simple_API/Shapes/Triangle.hpp"
#include "Simple_API/Shapes/Rectangle.hpp"
#include "Simple_API/Shapes/Circle.hpp"

#include "vec2.hpp"
#include "Circle_Object.hpp" // I need the struct template for quadtrees too

#include "Barnes-Hut/Quadtree.hpp"

#define GPU_OLD


struct Circle_Texture
{
    Circle_Texture(const float x, const float y,uint n_segments,const uint vb_position,const float radius = 1.f) : circ(x,y,n_segments,radius),vb_pos(vb_position){}
    Circle circ;
    uint vb_pos;
};

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
        xpos = 2*(xpos / WINDOW_WIDTH ) - 1;
        ypos = -2*(ypos / WINDOW_HEIGHT) + 1;

        global_mouse_x = (xpos - (cameraPosition.x)) * (1 / zoomLevel); //- cameraPosition.x / 2);
        global_mouse_y = (ypos - (cameraPosition.y)) * (1 / zoomLevel); //- cameraPosition.y / 2);
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
float vel_fac = 1.0f;
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
    if (key == GLFW_KEY_V) {
        if (action == GLFW_PRESS) vel_fac = 1.1f;
        else if (action == GLFW_RELEASE) vel_fac = 1.0f;
    }
    if (key == GLFW_KEY_B) {
        if (action == GLFW_PRESS) vel_fac = 0.9f;
        else if (action == GLFW_RELEASE) vel_fac =  1.0f;
    }
    if (key == GLFW_KEY_N) {
        if (action == GLFW_PRESS) vel_fac = -vel_fac;
        else if (action == GLFW_RELEASE) vel_fac = 1.0f;
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
    if (key == GLFW_KEY_R && action == GLFW_PRESS){
        zoomLevel = 1.0f;
        cameraPosition = glm::vec3(0.0f,0.0f,0.0f);
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
    int N_CIRCLES = 2000;
    bool state;
    if (argc >= 2 && argc <= 3){
        if(std::string(argv[1]) == "-cpu"){
            state = CPU_STATE;
            std::cout << "Set CPU State" << std::endl;
        }
        else if (std::string(argv[1]) == "-gpu"){
            state = GPU_STATE;
            std::cout << "Set GPU State" << std::endl;
        }
        if (argc == 3){
            N_CIRCLES = std::atoi(argv[2]);
        }
    }
    std::random_device dev;
    std::mt19937 rng(dev());
    std::normal_distribution<float> dist0(0,0.4f);
    std::normal_distribution<float> dist1(2,0.3f);
    std::normal_distribution<float> dist2(-2,0.3f);
    std::uniform_real_distribution<float> even_dist(-1.f,1.f);
    std::normal_distribution<float> mass_dist(100.f,50.f);
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
    Quadtree qtree(0.5f);
    const uint local_size = 64; // must be same as in compute shader
    const uint work_group_size = (N_CIRCLES + local_size - 1) / local_size;
    std::vector<Circle_Object> circs; circs.reserve(N_CIRCLES);
    std::vector<Circle_Texture> circle_textures; circle_textures.reserve(N_CIRCLES);
    float radius = 2.0f;   

    const uint VERTEX_SIZE = ((CIRCLE_SEGMENTS + 1) * 3);
    for (size_t i = 0; i < N_CIRCLES; i++) {
        float pos_x,pos_y;
        /*
        if (i < N_CIRCLES / 3){
            float angle = (2.0f * M_PI * i) / (N_CIRCLES / 3); 
            pos_x = radius * cos(angle) + vel_dist(rng) * 10; // just using velociy distribution to not have to create a new dist with similar values  
            pos_y = radius * sin(angle) + vel_dist(rng) * 10;  
        }else{
            pos_x = dist0(rng);
            pos_y = dist0(rng);
        }
        */
       
        if (i <= N_CIRCLES / 2){
            pos_x = dist0(rng) + 1.5f;
            pos_y = dist0(rng) + 1.5f;
        } else{
            pos_x = dist0(rng) - 1.5f;
            pos_y = dist0(rng) - 1.5f;
        }
        
       /*
        if (i < N_CIRCLES / 2){
            pos_x = even_dist(rng) * 4;
            pos_y = 0;

        }else{
            pos_y = even_dist(rng) * 4;
            pos_x = 0;
        }
        */
        
        
       
        float mass = mass_dist(rng); 
        vec2f random_velocity = vec2f(vel_dist(rng), vel_dist(rng));
        circs.emplace_back(pos_x,pos_y,mass,vec2f{ pos_y / 5 + random_velocity.x, -pos_x / 5 + random_velocity.y}); 
        circle_textures.emplace_back(pos_x,pos_y,CIRCLE_SEGMENTS,i * VERTEX_SIZE * sizeof(float),mass * MASS_TO_RADIUS);
    }   


    
    const uint circ_pos_size = N_CIRCLES * VERTEX_SIZE ;// positions and one float for vel
    const uint circ_idx_size = N_CIRCLES * ((CIRCLE_SEGMENTS * 3));

    std::vector<float> circ_metadata; circ_metadata.reserve(circ_pos_size);
    std::vector<uint> circ_indices; circ_indices.reserve(circ_idx_size);

    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<float>& positions = circle_textures[i].circ.get_positions();
        for(size_t i = 0; i < positions.size(); i += 2){
            circ_metadata.emplace_back(positions[i]);
            circ_metadata.emplace_back(positions[i + 1]);
            circ_metadata.emplace_back(0.00f);//initial velocity
        }
    }
    for (int i = 0; i < N_CIRCLES;i++){
        const std::vector<uint>& indices = circle_textures[i].circ.get_indices();
        for (int j = 0; j < (CIRCLE_SEGMENTS * 3); j++) {
            circ_indices[(i * (CIRCLE_SEGMENTS * 3)) + j] = indices[j] + (i * ((CIRCLE_SEGMENTS + 1)));
        }
    }
    VertexBuffer circ_vb(circ_metadata.data(),circ_pos_size * sizeof(float));
    IndexBuffer circ_ibo(circ_indices.data(),circ_idx_size);
    VertexArray circ_vao;
    VertexBufferLayout circ_vbl;
    circ_vbl.push(GL_FLOAT,2);
    circ_vbl.push(GL_FLOAT,1);
    circ_vao.add_buffer(circ_vb,circ_vbl);
    
    //Only works, becaouse circles are constant
    ShaderStorageBuffer ssbo(circs.data(),circs.size() * sizeof(Circle_Object));
    ssbo.bind_to(0);

    GlProgram prog({Shader("./shaders/vertex.shader", GL_VERTEX_SHADER).get_shader_ID(),
                    Shader("./shaders/fragment.shader", GL_FRAGMENT_SHADER).get_shader_ID()});
    prog.bind();
    prog.delete_shaders();
    
    #ifdef GPU_OLD
    GlProgram compute_prog({Shader("./shaders/compute.shader",GL_COMPUTE_SHADER).get_shader_ID()});
    #else
        GlProgram compute_prog({Shader("./shaders/compute2.shader",GL_COMPUTE_SHADER).get_shader_ID()});
    #endif
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
                

                #ifndef GPU_OLD
                //build the quadtree
                Quad quad;
                quad = quad.initialize(circs);
                qtree.reset(quad);
                for (auto& circ : circs){
                    qtree.insert_body(circ);
                }
                qtree.update_mass_centers();

                ShaderStorageBuffer ssbo_nodes(qtree.nodes.data(), qtree.nodes.size() * sizeof(Quadtree::Node));
                ssbo_nodes.bind_to(1);
                #endif
                ssbo.update_data(circs.data(),circs.size() * sizeof(Circle_Object));
                glCall(glDispatchCompute(work_group_size, 1, 1));  
                glCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)); 

                Circle_Object* data = (Circle_Object*)glMapBuffer(GL_SHADER_STORAGE_BUFFER,GL_READ_ONLY);
                if (data){
                    for (uint i = 0; i < N_CIRCLES;++i){
                        circs[i].vel = data[i].vel;
                        circs[i].mass = data[i].mass;
                        circle_textures[i].circ.set_radius(circs[i].mass * MASS_TO_RADIUS);
                    }
                    glCall(glUnmapBuffer(GL_SHADER_STORAGE_BUFFER));
                }
            }
            else if (state == CPU_STATE){
                //build the quadtree
                Quad quad;
                quad = quad.initialize(circs);
                qtree.reset(quad);

                for (auto& circ : circs){
                    qtree.insert_body(circ);
                }
                
                qtree.update_mass_centers();
                /*
                for (auto& circ : circs){
                    circ.force = qtree.apply_force(circ);
                }*/
                for (int i = 0; i < N_CIRCLES; ++i) {
                    circs[i].force = qtree.apply_force(circs[i]);
                }


            }
            
            for (int i = 0; i < N_CIRCLES; ++i) {
                circs[i].vel *= vel_fac;
                if (state != GPU_STATE){
                    circs[i].vel += (circs[i].force / circs[i].mass) * calculation_dt;
                    circs[i].force.x = 0;
                    circs[i].force.y = 0;
                }
                if(global_mouse_x != MOUSE_NOTHING_POS){
                    vec2f dist = (vec2f(global_mouse_x,global_mouse_y) - circs[i].pos);
                    circs[i].vel += (dist) / (circs[i].mass * 0.01f) * calculation_dt;
                    circs[i].vel = circs[i].vel /  1.1f;
                }
                circs[i].pos += circs[i].vel * calculation_dt;
                circle_textures[i].circ.set_pos(circs[i].pos.x,circs[i].pos.y);

                const std::vector<float>& positions = circle_textures[i].circ.get_positions();
                std::vector<float> interleaved_data;
                interleaved_data.reserve(positions.size() + positions.size() / 2);

                const float vel_abs = circs[i].vel.abs();
                for (size_t j = 0; j < positions.size(); j += 2){
                    interleaved_data.push_back(positions[j]);
                    interleaved_data.push_back(positions[j + 1] );
                    interleaved_data.push_back(vel_abs);
                }
                circ_vb.bind();

                glCall(glBufferSubData(GL_ARRAY_BUFFER,circle_textures[i].vb_pos,interleaved_data.size() * sizeof(float),interleaved_data.data()));

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


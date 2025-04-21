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

#include "Simple_API/Camera.hpp"

#include "vec2.hpp"
#include "Circle_Object.hpp" // I need the struct template for quadtrees too

#include "Barnes-Hut/Quadtree.hpp"

#define GPU_OLD

struct Vertex {
    vec2f pos;
};
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
Camera cam;
//glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 0.0f);
//float zoomLevel = 1.0f;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{   
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        xpos = 2*(xpos / WINDOW_WIDTH ) - 1;
        ypos = -2*(ypos / WINDOW_HEIGHT) + 1;

        global_mouse_x = (xpos - (cam.get_position().x)) * (1 / cam.get_zoom()); //- cameraPosition.x / 2);
        global_mouse_y = (ypos - (cam.get_position().y)) * (1 / cam.get_zoom()); //- cameraPosition.y / 2);
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        global_mouse_x = MOUSE_NOTHING_POS;
        global_mouse_y = MOUSE_NOTHING_POS;
    }
}

std::pair<float, float> generateRandomPoint(float cx, float cy, float r) {
    // Generate a random angle between 0 and 2π
    float angle = (rand() / (float)RAND_MAX) * 2 * M_PI;

    // Generate a random radius between 0 and r, but we want a uniform distribution across the area,
    // so we take the square root of a random number to ensure uniformity in 2D space.
    float radius = sqrt((rand() / (float)RAND_MAX)) * r;

    // Convert to Cartesian coordinates
    float x = cx + radius * cos(angle);
    float y = cy + radius * sin(angle);

    return {x, y};
}

std::vector<vec2f> gen_spiral(uint n_points,float a,float b,float spacing, float phase,std::mt19937 rng) {
    std::vector<vec2f> points{};
    float theta = 0.0;

    std::uniform_real_distribution<float> noiseDist(-0.08, 0.08);

    for (int i = 0; i < n_points; i++) {
        float r = spacing * theta;
        float x = a + r * cos(theta + phase);
        float y = b + r * sin(theta + phase);

        float noiseX = noiseDist(rng);
        float noiseY = noiseDist(rng);
        points.push_back({x + noiseX, y + noiseY});

        theta += 0.03; // Adjust for density of the spiral
    }

    return points;
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
        cam.set_zoom(1.f);
        cam.set_position(glm::vec2(0.f,0.f));
    }
}

int main(int argc, char* argv[]) {
    uint N_CIRCLES = 2000;
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
    std::normal_distribution<float> mass_dist(250.f,90.f);
    std::uniform_real_distribution<float> vel_dist(-0.008f, 0.008f);
    std::uniform_real_distribution<float> angleDist(0, 2 * M_PI);
    std::uniform_real_distribution<float> radiusDist(0, 1.f);
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
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello OpenGL", NULL, NULL);
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
    std::vector<Circle_Texture> circle_textures; circle_textures.reserve(1);

    const uint VERTEX_SIZE = ((CIRCLE_SEGMENTS + 1) * 2);

    for (size_t i = 0; i < N_CIRCLES ; i++) {
        float pos_x,pos_y;
        float mass = mass_dist(rng); 
        float radius = MASS_TO_RADIUS * mass;
        vec2f vel {0.0,0.0};
        //std::pair<float,float> p;
        pos_x = dist0(rng);
        pos_y = dist0(rng);
        if (i < N_CIRCLES / 2){
            pos_x += 25.f;
            if (i < N_CIRCLES / 4){
                pos_y += 25.f;
            }else{
                pos_y -= 25.f;
            }
        } else {
            pos_x -= 25.f;
            if (i < 3 * N_CIRCLES / 4){
                pos_y += 25.f;
            }else{ 
                pos_y -= 25.f;
            }
        }
        
        vel = vec2f(-pos_y,pos_x) * 0.06f;

        //vec2f random_velocity = vec2f(vel_dist(rng), vel_dist(rng));
        circs.emplace_back(pos_x,pos_y,mass,vel); 
    }   

    // the instance circle
    circle_textures.emplace_back(0.0f,0.0f,CIRCLE_SEGMENTS,0,1.f);

   

    const uint vertices_per_circ = CIRCLE_SEGMENTS + 1; 
    const uint n_vertices =  N_CIRCLES * (vertices_per_circ);
    const uint n_indices =  3 * CIRCLE_SEGMENTS;


    VertexBuffer circ_vb((void*) circle_textures[0].circ.get_positions().data(),circle_textures[0].circ.get_positions().size() * sizeof(float));
    IndexBuffer circ_ibo(circle_textures[0].circ.get_indices().data(),circle_textures[0].circ.get_indices().size() * sizeof(uint));
    VertexArray circ_vao;
    VertexBufferLayout circ_vbl;
    circ_vbl.push(GL_FLOAT,2);
    circ_vao.add_buffer(circ_vb,circ_vbl);

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
            
            std::string title = "Hello OpenGL at: " + std::to_string((1/dt)) + " FPS" + (paused ? " (paused)":  ""); 
            glfwSetWindowTitle(window, title.c_str());
            frame_count = 0;  
        }
        cam.update(W_Pressed,A_Pressed,S_Pressed,D_Pressed,Up_Pressed,Down_Pressed);
        renderer.clear();

        /* Camera*/
        glm::mat4 projection = cam.get_projection_mat();
        
        glm::mat4 view = cam.get_view_mat();

        prog.bind();
        prog.set_uniform_mat4f("u_projection",1,GL_FALSE,glm::value_ptr(projection));
        prog.set_uniform_mat4f("u_view",1,GL_FALSE,glm::value_ptr(view));
        //prog.set_uniform_1ui("vertices_per_circ",vertices_per_circ);
        
        if (!paused){
            if (state == GPU_STATE){
                compute_prog.bind();
                //compute_prog.set_uniform_1ui("data_len",N_CIRCLES);
                compute_prog.set_uniform_1ui("vertices_per_circ",vertices_per_circ);
                glCall(glDispatchCompute(work_group_size, 1, 1));  
                glCall(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT)); 

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
            if (state != GPU_STATE){

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
                   
                }
            }
        }
        /* Render here */
        renderer.draw_instanced(circ_vao,circ_ibo,prog,GL_TRIANGLES,N_CIRCLES);
        /* Swap front and back buffers */
        glCall(glfwSwapBuffers(window));
        /* Poll for and process events */
        glCall(glfwPollEvents());

    }

    glfwTerminate();
    return 0;
}

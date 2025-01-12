// GLAD First

#include <glad/glad.h>

// GLFW3 Second

#include <GLFW/glfw3.h>

// C++ Libraries

#include <algorithm>
#include <bits/stdc++.h>
#include <boost/preprocessor/stringize.hpp>
#include <ctime>
#include <iostream>

// OpenGL Mathematics

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ImGui Libraries

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// My Own Libraries

#include "buffer_handler.hpp"
#include "matrix_handler.hpp"
#include "read_data.cpp"
#include "shader_handler.hpp"

template <typename arr_v, typename arr_f>
void read_data(arr_v &vertices_arr, arr_f &faces_arr, std::string file_name);

void read_off_file_attrib(unsigned int &vertices, unsigned int &facets,
                          unsigned int &edges, std::string file_name);
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL Callback: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

int main() {
  std::cout << "Hello World!" << std::endl;

  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create Window Object
  GLFWwindow *window = glfwCreateWindow(800, 600, "FDVS", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  std::cerr << "GLFW WINDOW ESTABLISHED" << std::endl;

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize glad" << std::endl;
    return -1;
  }

// Setup My Shader Program
#ifdef CMAKE_CURRENT_SOURCE_DIR
  std::string root_directory = BOOST_PP_STRINGIZE(CMAKE_CURRENT_SOURCE_DIR);
  std::string vPath = root_directory;
  std::string fPath = root_directory;
  vPath.append("/src/shader.vs");
  fPath.append("/src/shader.fs");
  std::cerr << fPath << std::endl;
  Shader MyShader(vPath.c_str(), fPath.c_str());
#else
  return -1;
#error "DEFINE CMAKE_CURRENT_SOURCE_DIR"
#endif

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;            // Enable Gamepad Controls
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch

  // Setup Dear ImGui Style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install
                     // GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init("#version 330 core");

  // Set ImGui State
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  // Define the Vertices and Faces
  unsigned int num_of_vertices = 8, num_of_faces = 8, num_of_edges = 0;

  // Draw cube
  std::vector<float> vertices{

      -1.0f, -1.0f, 0.0f, // bottom left
      1.0f, 1.0f, 0.0f,   // top right
      -1.0f, 1.0f, 0.0f,  // top left
      1.0f, -1.0f, 0.0f,  // bottom right

      // Same thing in the back

      -1.0f, -1.0f, 3.0f, // bottom left
      1.0f, 1.0f, 3.0f,   // top right
      -1.0f, 1.0f, 3.0f,  // top left
      1.0f, -1.0f, 3.0f   // bottom right
  };
  std::vector<unsigned int> faces{0, 1, 3, 1, 2, 0, 4, 5, 7, 5, 6, 4,
                                  0, 4, 2, 2, 6, 4, 3, 7, 1, 1, 5, 7};
  // test_rectangle
  /*
  std::vector<float> vertices{
      -0.9f, -0.9f, 0.0f, // bottom left
      0.9f,  0.9f,  0.0f, // top right
      -0.9f, 0.9f,  0.0f, // top left
      0.9f,  -0.9f, 0.0f  // bottom right
  };
  std::vector<unsigned int> faces{0, 1, 3, 1, 2, 0};
  */

  /*
  read_off_file_attrib(num_of_vertices, num_of_faces, num_of_edges,
                       "output.off");
  std::cerr << num_of_faces << std::endl;
  std::vector<float> vertices;
  std::vector<unsigned int> faces;
  read_data<std::vector<float>, std::vector<unsigned int>>(vertices, faces,
                                                           "output.off");
  std::cerr << vertices[10] << " " << faces[3] << std::endl;
  */
  // TODO: Add Error Checking at This Step

  // Set the VAO, VBO, EBO, FBO, RBO
  unsigned int FBO_;
  glGenFramebuffers(1, &FBO_);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO_);

  GLint drawFboId = 0, readFboId = 0;
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFboId);
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFboId);

  if (drawFboId == FBO_) {
    std::cerr << "FBO successfully bound." << std::endl;
  } else {
    std::cerr << "User FBO not successfully bound" << std::endl;
  }
  if (readFboId == FBO_) {
    std::cerr << "FBO successfully bound." << std::endl;
  } else {
    std::cerr << "User FBO not successfully bound" << std::endl;
  }

  unsigned int texture_id_;
  glGenTextures(1, &texture_id_);
  glBindTexture(GL_TEXTURE_2D, texture_id_);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         texture_id_, 0);

  unsigned int RBO_;
  glGenRenderbuffers(1, &RBO_);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, RBO_);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n";

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  unsigned int VBO;
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
               &vertices.front(), GL_STATIC_DRAW);

  unsigned int EBO;
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int),
               &faces.front(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  // Timer
  unsigned int counter = 0;
  time_t start_time, end_time;
  time(&start_time);
  float lastTime = glfwGetTime();

  // Wireframe Mode
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
  unsigned int s_x = 800, s_y = 600;
  glfwSetCursorPos(window, (float)s_x / 2, (float)s_y / 2);
  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glfwPollEvents();

    // Delay Rendering if Window Not In Focus
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED != 0)) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // Custom Graph Window
    // Get Context Information
    ImGui::Begin("Graph");
    const ImVec2 size_avail = ImGui::GetContentRegionAvail();
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    std::string size_availx = std::to_string(size_avail.x);
    std::string size_availy = std::to_string(size_avail.y);
    std::string posx = std::to_string(pos.x);
    std::string posy = std::to_string(pos.y);
    s_x = size_avail.x;
    s_y = size_avail.y;

    // Rescale Framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s_x, s_y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture_id_, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, RBO_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, s_x, s_y);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO_);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::GetWindowDrawList()->AddImage(
        (void *)texture_id_, ImVec2(pos.x, pos.y),
        ImVec2(pos.x + s_x, pos.y + s_y), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();

    // 3d Transformations
    /*
    glm::vec3 position = glm::vec3(0, 0, 8);
    float horizontalAngle = 3.14f;
    float verticalAngle = 0.0f;
    float initialFoV = 45.0f;
    float speed = 3.0f;
    float mouseSpeed = 0.05f;

    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    lastTime = glfwGetTime();

    // read mouse position
    double xpos, ypos;
    // GLint windowWidth, windowHeight;
    glfwGetCursorPos(window, &xpos, &ypos);
    // glfwGetWindowSize(window, &windowWidth, &windowHeight);
    //	glfwSetCursorPos(window, (float)windowWidth/2,(float)windowHeight/2);
    horizontalAngle += mouseSpeed * deltaTime * float((float)s_x / 2 - xpos);
    verticalAngle += mouseSpeed * deltaTime * float((float)s_y / 2 - ypos);
    glm::vec3 direction =
        glm::vec3(cos(verticalAngle) * sin(horizontalAngle), sin(verticalAngle),
                  cos(verticalAngle) * cos(horizontalAngle));
    glm::vec3 right_vector = glm::vec3(sin(horizontalAngle - 3.14f / 2.0f), 0,
                                       cos(horizontalAngle - 3.14f / 2.0f));
    glm::vec3 up = glm::cross(right_vector, direction);
*/
    glBindFramebuffer(GL_FRAMEBUFFER, FBO_);
    glViewport(0, 0, s_x, s_y);
    MyShader.use();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    // Setup Transformation Matrices
    glm::mat4 ModelMatrix = glm::mat4(0.9f);
    glm::mat4 ViewMatrix = glm::mat4(1.0f);
    glm::mat4 ProjectionMatrix = glm::mat4(1.0f);

    ProjectionMatrix =
        glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 100.0f);
    glm::mat4 pmatrix =
        PerspectiveFrustrum(1.5f, -1.5f, 1.5f, -1.5f, 0.01f, 100.0f);
    for (auto i = 0; i < 4; i++) {
      for (auto j = 0; j < 4; j++)
        std::cerr << "GLM Version: " << pmatrix[i][j]
                  << "\t\t My version: " << ProjectionMatrix[i][j] << std::endl;
      ;
    }
    ViewMatrix = glm::translate(ViewMatrix, glm::vec3(0.0f, 0.0f, -10.0f));

    int modelID = glGetUniformLocation(MyShader.ID, "model");
    int viewID = glGetUniformLocation(MyShader.ID, "view");
    int projectionID = glGetUniformLocation(MyShader.ID, "projection");

    glUniformMatrix4fv(modelID, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glUniformMatrix4fv(viewID, 1, GL_FALSE, glm::value_ptr(ViewMatrix));
    glUniformMatrix4fv(projectionID, 1, GL_FALSE,
                       glm::value_ptr(ProjectionMatrix));

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, num_of_faces * 3, GL_UNSIGNED_INT, 0);
    glFlush();
    glUseProgram(0);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Render
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                 clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap Buffer
    glfwSwapBuffers(window);
    glfwPollEvents();

    counter++;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  time(&end_time);

  std::cerr << end_time - start_time << "Seconds"
            << (float)counter / (end_time - start_time) << "FPS" << std::endl;
  std::cerr << counter
            << " FRAMES RENDERED.  RENDER LOOP COMPLETED. TERMINATING "
               "APPLICATION..."
            << std::endl;
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteFramebuffers(1, &FBO_);
  glDeleteProgram(MyShader.ID);
  glfwDestroyWindow(window);
  glfwTerminate();
}

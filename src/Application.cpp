#include "Application.hpp"
/*
class Application {
public:
  Application();
  bool isOpen();
  void refresh();
  void close();

private:
  int app_x_, app_y_;
  GLFWwindow *window;

  void CreateWindow();
  void SetupImGui();
  void GraphWindow(FBO fb);
  void Render();
  void Cleanup();
} */

Application::Application() {
  CreateWindow();
  SetupImGui();

  // Get Data from Off file
  Data::ReadOffFileAttrib(data.num_of_vertices, data.num_of_faces,
                          data.num_of_edges, "output.off");
  Data::ReadOffData(data.vertices, data.faces, "output.off");

  // Setup Shaders
#ifdef CMAKE_CURRENT_SOURCE_DIR
  std::string root_directory = BOOST_PP_STRINGIZE(CMAKE_CURRENT_SOURCE_DIR);
  std::string vPath = root_directory;
  std::string fPath = root_directory;
  vPath.append("/src/shader.vs");
  fPath.append("/src/shader.fs");
  std::cerr << fPath << std::endl;
  shader = std::make_shared<Shader>(vPath.c_str(), fPath.c_str());
#else
  throw SHADER_CONSTRUCTION_ERROR{};
#error "DEFINE CMAKE_CURRENT_SOURCE_DIR"
#endif

  // Create Graph Window FBO Target
  graph_fbo_ = std::make_shared<Buffers::FBO>(800, 600);

  // Setup Graphics Elements
  graph = std::make_shared<Graphics::TriangleMesh>(shader, data, graph_fbo_);
}

bool Application::isOpen() { return !glfwWindowShouldClose(window); }

void Application::Refresh() {
  try {
    InitializeNewFrame();
  } catch (WINDOW_NOT_IN_FOCUS wnif) {
    throw;
  }

  // Draw Each Window
  ImVec2 windowSize = GraphWindow(*graph_fbo_);
  // Update the Contents of Each Window
  graph->drawToBuffer(windowSize.x, windowSize.y);

  Render();
}

void Application::Close() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glDeleteProgram(shader->ID);
  glfwDestroyWindow(window);
  glfwTerminate();
}

void Application::InitializeNewFrame() {
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glfwPollEvents();

  // Delay Rendering if Window Not In Focus
  if (glfwGetWindowAttrib(window, GLFW_ICONIFIED != 0)) {
    ImGui_ImplGlfw_Sleep(10);
    throw WINDOW_NOT_IN_FOCUS{};
  }

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}

void Application::CreateWindow() {
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create Window Object
  window = glfwCreateWindow(800, 600, "FDVS", NULL, NULL);
  if (window == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    throw GLFW_WINDOW_CREATION_FAILURE{};
  }
  std::cerr << "GLFW WINDOW ESTABLISHED" << std::endl;

  glfwMakeContextCurrent(window);

  // Use glfwSwapInterval(0); to max out frame rate
  glfwSwapInterval(1);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize glad" << std::endl;
    throw GLAD_LOADING_ERROR{};
  }

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
}

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar *message, const void *userParam) {
  fprintf(stderr,
          "GL Callback: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

void Application::SetupImGui() {

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  io = &ImGui::GetIO();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install
                     // GLFW callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init("#version 330 core");

  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad;             // Enable Gamepad Controls
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable; // IF using Docking Branch

  // Setup Dear ImGui Style
  ImGui::StyleColorsDark();
}

const ImVec2 Application::GraphWindow(Buffers::FBO &fb) {
  ImGui::Begin("Graph");
  const ImVec2 size_avail = ImGui::GetContentRegionAvail();
  const ImVec2 pos = ImGui::GetCursorScreenPos();

  std::string upper_left =
      "(" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")";
  std::string bottom_right = "(" + std::to_string(pos.x + size_avail.x) + ", " +
                             std::to_string(pos.y + size_avail.x) + ")";

  ImGui::Text(upper_left.c_str());
  ImGui::Text(bottom_right.c_str());

  // Rescale Framebuffer
  fb.Rescale((GLsizei)size_avail.x, (GLsizei)size_avail.y);

  ImGui::GetWindowDrawList()->AddImage(
      (void *)fb.GetAttachedTextureID(), ImVec2(pos.x, pos.y),
      ImVec2(pos.x + size_avail.x, pos.y + size_avail.y), ImVec2(0, 0),
      ImVec2(1, 1));

  ImGui::End();
  return size_avail;
}

void Application::Render() {
  ImGui::Render();
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  // glViewport(0, 0, display_w, display_h);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap Buffer
  glfwSwapBuffers(window);
  glfwPollEvents();

  glFlush();
}

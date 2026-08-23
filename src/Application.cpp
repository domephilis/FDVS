#include "Application.hpp"

Application::Application() {
  CreateWindow();
  SetupImGui();

  // Setup Shaders
#ifdef CMAKE_CURRENT_SOURCE_DIR
  std::string root_directory = BOOST_PP_STRINGIZE(CMAKE_CURRENT_SOURCE_DIR);
  std::string vPath = root_directory;
  std::string fPath = root_directory;
  vPath.append("/src/shaders/shader.vs");
  fPath.append("/src/shaders/shader.fs");
  std::cerr << fPath << std::endl;
  shader = std::make_shared<Shader>(vPath.c_str(), fPath.c_str());
#else
  throw SHADER_CONSTRUCTION_ERROR{};
#error "DEFINE CMAKE_CURRENT_SOURCE_DIR"
#endif

  // Configure IO Controller
  io_ctr_ = std::make_shared<Events::Controller>(window);
  io_ctr_->contexts.push_back(main_context);
  glfwSetWindowUserPointer(window, (void *)io_ctr_.get());
  panels.emplace(std::make_pair(
      "Graph",
      std::dynamic_pointer_cast<Windowing::Panel>(
          std::make_shared<Windowing::GraphPanel>(shader, io_ctr_))));
  panels.emplace(std::make_pair(
      "Config", std::dynamic_pointer_cast<Windowing::Panel>(
                    std::make_shared<Windowing::ConfigPanel>(
                        io_ctr_,
                        std::dynamic_pointer_cast<Windowing::GraphPanel>(
                            panels["Graph"])))));
}

bool Application::isOpen() { return !glfwWindowShouldClose(window); }

void Application::Refresh() {
  try {
    InitializeNewFrame();
  } catch (WINDOW_NOT_IN_FOCUS wnif) {
    throw;
  }

  RenderMain();

  // Draw Each Window
  for (const auto &[key, panel] : panels)
    panel->Render();

  // Swap Buffer
  // glfwPollEvents();

  ImGui::Render();
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  glViewport(0, 0, display_w, display_h);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  glfwSwapBuffers(window);
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

  ImGui::DockSpaceOverViewport();
}

void Application::CreateWindow() {
  // Initialize GLFW
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

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
  main_context = ImGui::CreateContext();
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
  Themes::SetMoonlightStyle();
}

void Application::RenderMain() {
  ImVec4 clear_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w,
               clear_color.z * clear_color.w, clear_color.w);
  glClear(GL_COLOR_BUFFER_BIT);
}

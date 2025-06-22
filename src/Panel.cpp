#include "Panel.hpp"

Windowing::GraphPanel::GraphPanel(GLFWwindow *window,
                                  std::shared_ptr<Shader> in_shader,
                                  std::shared_ptr<Events::Controller> io_ctr) {
  shader = in_shader;
  io_ctr_ = io_ctr;

  z_scaling_ = 1.0f;

  // Create ImGui Context
  context = ImGui::CreateContext();
  {
    ImGuiContext *g = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(context);
    this->window = window;
    ImGui_ImplGlfw_InitForOpenGL(this->window, false);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    // ImGuiIO io = ImGui::GetIO();
    // io.ConfigWindowsMoveFromTitleBarOnly = true;
    ImGui::SetCurrentContext(g);
  }

  // Construct from a file
  data_ = std::make_shared<Data::OffMeshData>("output.off");

  std::array<float, 3> max({data_->max[0], data_->max[0], data_->max[0]});
  depth = *std::max_element(max.begin(), max.end());

  // Configure MatrixStack
  mv_stack_ = std::make_unique<Matrices::MatrixStack>("modelview", shader);
  proj_stack_ = std::make_unique<Matrices::MatrixStack>("projection", shader);
  camera_ = std::make_unique<Matrices::CameraFrame>(context);

  io_ctr_->contexts.push_back(context);

  io_ctr_->k_publisher_->addSubscriber(
      std::dynamic_pointer_cast<Events::KeyboardSubscriber>(
          camera_->k_subscription_));
  io_ctr_->m_publisher_->addSubscriber(camera_->m_subscription_);
  io_ctr_->s_publisher_->addSubscriber(camera_->s_subscription_);

  // Create Graph Window FBO Target
  graph_fbo_ = std::make_shared<Buffers::FBO>(800, 600);

  // Setup Graphics Elements
  graph = std::make_unique<Graphics::TriangleMesh>(shader, data_, graph_fbo_);
  x_axis_ = std::make_unique<Graphics::Line>(glm::vec3(0.0f, 0.0f, 0.0f),
                                             glm::vec3(1.1 * depth, 0.0f, 0.0f),
                                             shader, graph_fbo_);
  y_axis_ = std::make_unique<Graphics::Line>(glm::vec3(0.0f, 0.0f, 0.0f),
                                             glm::vec3(0.0f, 1.1 * depth, 0.0f),
                                             shader, graph_fbo_);
  z_axis_ = std::make_unique<Graphics::Line>(
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, -1.1 * depth * z_scaling_), shader, graph_fbo_);
}

void Windowing::GraphPanel::Render() {

  ImGuiContext *g = ImGui::GetCurrentContext();
  ImGui::SetCurrentContext(context);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Graph");

  const ImVec2 size_avail = ImGui::GetContentRegionAvail();
  const ImVec2 pos = ImGui::GetCursorScreenPos();

  // First get the current window reference
  ImGuiWindow *Window = ImGui::GetCurrentWindow();

  ImRect rect(pos.x + 5, pos.y + 5, pos.x + size_avail.x - 10,
              pos.y + size_avail.y - 10);

  // Check if the mouse is in that rect
  if (rect.Contains(ImGui::GetMousePos()) && ImGui::IsWindowFocused()) {
    Window->Flags |= ImGuiWindowFlags_NoMove;
    camera_->m_subscription_->SetUserFlag(true);
    camera_->k_subscription_->SetUserFlag(true);
    camera_->s_subscription_->SetUserFlag(true);
  } else {
    Window->Flags &= ~ImGuiWindowFlags_NoMove;
    camera_->m_subscription_->SetUserFlag(false);
    camera_->k_subscription_->SetUserFlag(false);
    camera_->s_subscription_->SetUserFlag(false);
  }

  std::string upper_left =
      "(" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")";
  std::string bottom_right = "(" + std::to_string(pos.x + size_avail.x) + ", " +
                             std::to_string(pos.y + size_avail.x) + ")";

  ImGui::Text(upper_left.c_str());
  ImGui::Text(bottom_right.c_str());

  // Rescale Framebuffer
  graph_fbo_->Rescale((GLsizei)size_avail.x, (GLsizei)size_avail.y);

  ImGui::GetWindowDrawList()->AddImage(
      (void *)graph_fbo_->GetAttachedTextureID(), ImVec2(pos.x, pos.y),
      ImVec2(pos.x + size_avail.x, pos.y + size_avail.y), ImVec2(0, 0),
      ImVec2(1, 1));

  ImGui::End();

  // Antialiasing
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_POLYGON_SMOOTH);

  int success = 0;
  glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
  shader->use();
  glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);

  // Set Camera
  mv_stack_->pushMatrix(glm::mat4(1.0f));
  camera_->m_subscription_->UpdateWindowCentre(pos.x + size_avail.x / 2,
                                               pos.y + size_avail.y / 2);
  mv_stack_->pushMatrix(camera_->GetTransformMatrix());

  // Draw Graph
  // Reminder: Call shader->use() everytime you pop a matrix
  mv_stack_->pushProduct(
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f)));
  proj_stack_->pushMatrix(glm::perspective(
      glm::radians(45.0f), (float)size_avail.x / (float)size_avail.y, 0.1f,
      5 * depth * static_cast<float>(z_scaling_)));
  mv_stack_->popMatrix();
  proj_stack_->popMatrix();
  graph->drawToBuffer(size_avail.x, size_avail.y);
  x_axis_->drawToBuffer(size_avail.x, size_avail.y);
  y_axis_->drawToBuffer(size_avail.x, size_avail.y);
  z_axis_->drawToBuffer(size_avail.x, size_avail.y);

  glUseProgram(0);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGui::SetCurrentContext(g);
}

Windowing::ConfigPanel::ConfigPanel(GLFWwindow *window,
                                    std::shared_ptr<Events::Controller> io_ctr,
                                    std::shared_ptr<GraphPanel> graph_panel)
    : window_(window), io_ctr_(io_ctr), graph_panel_(graph_panel) {

  // Context Creation
  prev_context_ = ImGui::GetCurrentContext();
  context_ = ImGui::CreateContext();
  ImGui::SetCurrentContext(context_);
  ImGui_ImplGlfw_InitForOpenGL(window_, false);
  ImGui_ImplOpenGL3_Init("#version 330 core");
  io = ImGui::GetIO();
  ImGui::SetCurrentContext(prev_context_);

  // Events Subscriptions
  k_subscription_ = std::make_shared<KSubscription>(context_);
  m_subscription_ = std::make_shared<MSubscription>(context_);
  s_subscription_ = std::make_shared<SSubscription>(context_);

  // Link to Controller
  io_ctr_->contexts.push_back(context_);
  io_ctr_->k_publisher_->addSubscriber(
      std::dynamic_pointer_cast<Events::KeyboardSubscriber>(
          this->k_subscription_));
  io_ctr_->m_publisher_->addSubscriber(
      std::dynamic_pointer_cast<Events::MouseSubscriber>(
          this->m_subscription_));
  io_ctr_->s_publisher_->addSubscriber(
      std::dynamic_pointer_cast<Events::ScrollwheelSubscriber>(
          this->s_subscription_));
}

void Windowing::ConfigPanel::Render() {

  ImGui::SetCurrentContext(context_);

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Configuration Panel");
  ImGui::SeparatorText("Graph Configuration");
  const char *models[] = {"Black Scholes", "Binomial", "Monte Carlo"};
  static int model = 0;
  ImGui::Combo("Model", &model, models, IM_ARRAYSIZE(models));
  const char *variables[] = {"Spot Price",     "Strike Price",
                             "Time To Expiry", "Implied Volatility",
                             "Risk Free Rate", "Dividend Rate"};

  static int x_axis = 0;
  ImGui::Combo("X Axis Variable", &x_axis, variables, IM_ARRAYSIZE(variables));
  static double min_x = 0.0f, step_size_x = 0.0f;
  static unsigned int steps_x = 0;
  ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
  ImGui::InputScalar("X Start", ImGuiDataType_Double, &min_x, NULL, NULL, NULL,
                     flags);
  ImGui::InputScalar("X Step Size", ImGuiDataType_Double, &step_size_x, NULL,
                     NULL, NULL, flags);
  ImGui::InputScalar("X Steps", ImGuiDataType_U32, &steps_x, NULL, NULL, NULL,
                     flags);

  static int y_axis = 0;
  ImGui::Combo("Y Axis Variable", &y_axis, variables, IM_ARRAYSIZE(variables));
  static double min_y = 0.0f, step_size_y = 0.0f;
  static unsigned int steps_y = 0;
  ImGui::InputScalar("Y Start", ImGuiDataType_Double, &min_y, NULL, NULL, NULL,
                     flags);
  ImGui::InputScalar("Y Step Size", ImGuiDataType_Double, &step_size_y, NULL,
                     NULL, NULL, flags);
  ImGui::InputScalar("Y Steps", ImGuiDataType_U32, &steps_y, NULL, NULL, NULL,
                     flags);

  static int z_axis = 0;
  const char *pos_image_vars[] = {"Option Price", "Delta", "Gamma", "Theta",
                                  "Vega"};
  ImGui::Combo("Z Axis Variable", &z_axis, pos_image_vars,
               IM_ARRAYSIZE(pos_image_vars));

  // The x_axis, y_axis spots will be unpopulated and ignored
  static std::array<double, 6> arr;
  for (int i = 0; i < 6; i++) {
    if (i != x_axis && i != y_axis) {
      ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
      ImGui::InputScalar(variables[i], ImGuiDataType_Double, &arr[i], NULL,
                         NULL, NULL, flags);
    }
  }
  static double z_scaling = 0.0f;
  ImGui::InputScalar("Z Scale Factor", ImGuiDataType_Double, &z_scaling, NULL,
                     NULL, NULL, flags);

  std::map<std::string, double> params{
      std::make_pair(std::string(variables[x_axis]), -1),
      std::make_pair(std::string(variables[y_axis]), -1)};
  for (int i = 0; i < 6; i++) {
    if (i != x_axis && i != y_axis)
      params.insert(std::make_pair(std::string(variables[i]), arr[i]));
  }

  std::array<std::string, 2> to_vary{variables[x_axis], variables[y_axis]};

  typedef std::array<double, 2> Point2D;
  Models::BlackScholes bs(Models::PayoffType::Call, params, to_vary);
  graph_panel_->SetModel(
      std::make_shared<Data::Bounds2D>(Point2D{min_x, min_y}, steps_x, steps_y,
                                       step_size_x, step_size_y),
      bs.GetComputeFunction(pos_image_vars[z_axis]), z_scaling);
  auto f = bs.GetComputeFunction(pos_image_vars[z_axis]);

  if (ImGui::Button("Reload")) {
    graph_panel_->ReloadModel();
    graph_panel_->data_->ExportToOff("output_recalculated");
    std::cerr << "f(100, 100)" << f(100.0f, 100.0f) << std::endl;
    std::cerr << "f(150, 200)" << f(150.0f, 200.0f) << std::endl;
  }

  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  ImGui::SetCurrentContext(prev_context_);
}

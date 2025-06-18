#include "Panel.hpp"

Windowing::GraphPanel::GraphPanel(std::shared_ptr<Shader> in_shader,
                                  std::shared_ptr<Events::Controller> io_ctr) {
  shader = in_shader;
  io_ctr_ = io_ctr;

  // Construct from a file
  data_ = std::make_shared<Data::OffMeshData>("output.off");

  // Construct via a Function
  /*
  Models::BlackScholes bs_pricer(Data::PayoffType::Call,
                                 {{"Time To Expiry", 0.08f},
                                  {"Implied Volatility", 0.263f},
                                  {"Risk Free Rate", 0.05f},
                                  {"Dividend Rate", 0.05f}}
                                 {{"Spot Price", -1}, {"Strike Price", -1}});
  data_ = std::make_shared<Data::OffMeshData>(
      {{20.0f, 20.0f}, 250, 250, 2.0f, 2.0f},
  std::bind(&(Models::BlackScholes::Compute), &bs_pricer););
  */

  std::array<float, 3> max({data_->max[0], data_->max[0], data_->max[0]});
  depth = *std::max_element(max.begin(), max.end());

  // Configure MatrixStack
  mv_stack_ = std::make_unique<Matrices::MatrixStack>("modelview", shader);
  proj_stack_ = std::make_unique<Matrices::MatrixStack>("projection", shader);
  camera_ = std::make_unique<Matrices::CameraFrame>();

  io_ctr_->k_publisher_->addSubscriber(
      std::dynamic_pointer_cast<Events::KeyboardSubscriber>(
          camera_->k_subscription_));
  io_ctr_->m_publisher_->addSubscriber(camera_->m_subscription_);

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
      glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.1 * depth), shader,
      graph_fbo_);
}

void Windowing::GraphPanel::Render() {
  ImGui::Begin("Graph");

  /*
  if (ImGui::IsWindowFocused())
    camera_->k_subscription_->EnableKeyboardInput();
  else
    camera_->k_subscription_->DisableKeyboardInput();
  */

  const ImVec2 size_avail = ImGui::GetContentRegionAvail();
  const ImVec2 pos = ImGui::GetCursorScreenPos();

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

  shader->use();

  // Set Camera
  camera_->m_subscription_->UpdateWindowCentre(pos.x + size_avail.x / 2,
                                               pos.y + size_avail.y / 2);
  mv_stack_->pushMatrix(camera_->GetTransformMatrix());

  // Draw Graph
  mv_stack_->pushProduct(
      glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -20.0f)));
  proj_stack_->pushMatrix(glm::perspective(
      glm::radians(45.0f), (float)size_avail.x / (float)size_avail.y, 0.1f,
      5 * depth));
  mv_stack_->popMatrix();
  proj_stack_->popMatrix();
  graph->drawToBuffer(size_avail.x, size_avail.y);
  x_axis_->drawToBuffer(size_avail.x, size_avail.y);
  y_axis_->drawToBuffer(size_avail.x, size_avail.y);
  z_axis_->drawToBuffer(size_avail.x, size_avail.y);

  glUseProgram(0);
}

Windowing::ConfigPanel::ConfigPanel(std::shared_ptr<GraphPanel> graph_panel)
    : graph_panel_(graph_panel) {}

void Windowing::ConfigPanel::Render() {
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
  static float min_x = 0.0f, step_size_x = 0.0f;
  static unsigned int steps_x = 0;
  ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
  ImGui::InputScalar("X Start", ImGuiDataType_Float, &min_x, NULL, NULL, NULL,
                     flags);
  ImGui::InputScalar("X Step Size", ImGuiDataType_Float, &step_size_x, NULL,
                     NULL, NULL, flags);
  ImGui::InputScalar("X Steps", ImGuiDataType_U32, &steps_x, NULL, NULL, NULL,
                     flags);

  static int y_axis = 0;
  ImGui::Combo("Y Axis Variable", &y_axis, variables, IM_ARRAYSIZE(variables));
  static float min_y = 0.0f, step_size_y = 0.0f;
  static unsigned int steps_y = 0;
  ImGui::InputScalar("Y Start", ImGuiDataType_Float, &min_y, NULL, NULL, NULL,
                     flags);
  ImGui::InputScalar("Y Step Size", ImGuiDataType_Float, &step_size_y, NULL,
                     NULL, NULL, flags);
  ImGui::InputScalar("Y Steps", ImGuiDataType_U32, &steps_y, NULL, NULL, NULL,
                     flags);

  static int z_axis = 0;
  const char *pos_image_vars[] = {"Option Price", "Delta", "Gamma", "Theta",
                                  "Vega"};
  ImGui::Combo("Z Axis Variable", &z_axis, pos_image_vars,
               IM_ARRAYSIZE(pos_image_vars));

  // The x_axis, y_axis spots will be unpopulated and ignored
  static std::array<float, 6> arr;
  for (int i = 0; i < 6; i++) {
    if (i != x_axis && i != y_axis) {
      ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
      ImGui::InputScalar(variables[i], ImGuiDataType_Float, &arr[i], NULL, NULL,
                         NULL, flags);
    }
  }

  std::map<std::string, float> params{
      std::make_pair(std::string(variables[x_axis]), -1),
      std::make_pair(std::string(variables[y_axis]), -1)};
  for (int i = 0; i < 6; i++) {
    if (i != x_axis && i != y_axis)
      params.insert(std::make_pair(std::string(variables[i]), arr[i]));
  }

  std::array<std::string, 2> to_vary{variables[x_axis], variables[y_axis]};

  typedef std::array<float, 2> Point2D;
  Models::BlackScholes bs(Models::PayoffType::Call, params, to_vary);
  graph_panel_->SetModel(
      std::make_shared<Data::Bounds2D>(Point2D{min_x, min_y}, steps_x, steps_y,
                                       step_size_x, step_size_y),
      bs.GetComputeFunction());
  auto f = bs.GetComputeFunction();
  // Later on we can get delta gamma theta vega function if this works

  if (ImGui::Button("Reload")) {
    graph_panel_->ReloadModel();
    graph_panel_->data_->ExportToOff("output_recalculated");
    std::cerr << "f(100, 100)" << f(100.0f, 100.0f) << std::endl;
    std::cerr << "f(150, 200)" << f(150.0f, 200.0f) << std::endl;
  }

  ImGui::End();
}

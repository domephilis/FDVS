#include "Panel.hpp"

Windowing::GraphPanel::GraphPanel(std::shared_ptr<Shader> in_shader,
                                  std::shared_ptr<Events::Controller> io_ctr) {
  shader = in_shader;
  io_ctr_ = io_ctr;

  data_ = std::make_shared<Data::OffMeshData>("output.off");
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
}

void Windowing::GraphPanel::Render() {
  ImGui::Begin("Graph");

  if (ImGui::IsWindowFocused())
    camera_->k_subscription_->EnableKeyboardInput();
  else
    camera_->k_subscription_->DisableKeyboardInput();

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

  shader->use();

  // Set Camera
  camera_->m_subscription_->UpdateWindowCentre(pos.x + size_avail.x / 2,
                                               pos.y + size_avail.y / 2);
  mv_stack_->pushMatrix(camera_->GetTransformMatrix());

  // Draw Graph
  mv_stack_->pushProduct(glm::translate(glm::mat4(1.0f), -data_->centre));
  proj_stack_->pushMatrix(glm::perspective(
      glm::radians(45.0f), (float)size_avail.x / (float)size_avail.y, 0.1f,
      500.0f));
  mv_stack_->popMatrix();
  proj_stack_->popMatrix();
  graph->drawToBuffer(size_avail.x, size_avail.y);

  glUseProgram(0);
}

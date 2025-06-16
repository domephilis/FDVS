#ifndef FDVS_SRC_PANEL_HPP_
#define FDVS_SRC_PANEL_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_
// Glad first
#include <glad/glad.h>

// GLFW second
#include <GLFW/glfw3.h>
#endif // !GL_HEADERS_

#include "imgui.h"

#include <memory>

#include "Buffers.hpp"
#include "Data.hpp"
#include "Events.hpp"
#include "Graphics.hpp"
#include "MatrixHandler.hpp"
#include "ShaderHandler.hpp"

namespace Windowing {
class Panel {
public:
  virtual void Render() = 0;
  virtual ~Panel() {}
};

class GraphPanel : public Panel {
public:
  GraphPanel(std::shared_ptr<Shader> in_shader,
             std::shared_ptr<Events::Controller> io_ctr);
  void Render();
  ~GraphPanel() {}

private:
  std::shared_ptr<Shader> shader;
  std::shared_ptr<Events::Controller> io_ctr_;
  std::unique_ptr<Matrices::CameraFrame> camera_;
  std::unique_ptr<Matrices::MatrixStack> mv_stack_;
  std::unique_ptr<Matrices::MatrixStack> proj_stack_;

  std::shared_ptr<Buffers::FBO> graph_fbo_;
  std::unique_ptr<Graphics::TriangleMesh> graph;
  std::shared_ptr<Data::OffMeshData> data_;
};

} // namespace Windowing

#endif // !FDVS_SRC_PANEL_HPP_

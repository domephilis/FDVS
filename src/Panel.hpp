#ifndef FDVS_SRC_PANEL_HPP_
#define FDVS_SRC_PANEL_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_
// Glad first
#include <glad/glad.h>

// GLFW second
#include <GLFW/glfw3.h>
#endif // !GL_HEADERS_

#include <functional>
#include <memory>

#include "Buffers.hpp"
#include "Data.hpp"
#include "Events.hpp"
#include "Graphics.hpp"
#include "MatrixHandler.hpp"
#include "Models.hpp"
#include "ShaderHandler.hpp"

#include "imgui.h"

typedef std::array<float, 2> Point2D;

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
  void SetModel(std::shared_ptr<Data::Bounds2D> b,
                std::function<float(float, float)> f) {
    bounds = b;
    domain_R2_function_ = f;
  }
  void ReloadModel() {
    if (bounds != nullptr) {
      data_ = std::make_shared<Data::OffMeshData>(bounds, domain_R2_function_);
      graph->updateData(data_);
    } else {
      throw Data::INVALID_BOUNDS_EXCEPTION{};
    }
  }
  ~GraphPanel() {}

  std::shared_ptr<Data::OffMeshData> data_;

private:
  float depth;

  std::shared_ptr<Shader> shader;
  std::shared_ptr<Events::Controller> io_ctr_;
  std::unique_ptr<Matrices::CameraFrame> camera_;
  std::unique_ptr<Matrices::MatrixStack> mv_stack_;
  std::unique_ptr<Matrices::MatrixStack> proj_stack_;

  // Actual Graph
  std::shared_ptr<Buffers::FBO> graph_fbo_;
  std::shared_ptr<Data::Bounds2D> bounds;
  std::unique_ptr<Graphics::TriangleMesh> graph;
  // std::shared_ptr<Data::OffMeshData> data_;
  std::unique_ptr<Graphics::Line> x_axis_;
  std::unique_ptr<Graphics::Line> y_axis_;
  std::unique_ptr<Graphics::Line> z_axis_;
  std::function<float(float, float)> domain_R2_function_;
};

class ConfigPanel : public Panel {
public:
  ConfigPanel(std::shared_ptr<GraphPanel> graph_panel);
  void Render();
  ~ConfigPanel() {}

private:
  std::shared_ptr<GraphPanel> graph_panel_;
};

} // namespace Windowing

#endif // !FDVS_SRC_PANEL_HPP_

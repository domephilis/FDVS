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
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

typedef std::array<float, 2> Point2D;

namespace Windowing {

class Panel {
public:
  virtual void Render() = 0;
  virtual ~Panel() {}

protected:
  GLFWwindow *window;
  ImGuiContext *context;
};

class GraphPanel : public Panel {
public:
  GraphPanel(GLFWwindow *window, std::shared_ptr<Shader> in_shader,
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
  ConfigPanel(GLFWwindow *window, std::shared_ptr<Events::Controller> io_ctr,
              std::shared_ptr<GraphPanel> graph_panel);
  void Render();
  ~ConfigPanel() {}

  class KSubscription : public Events::KeyboardSubscriber {
  public:
    KSubscription(ImGuiContext *context) { context_ = context; }
    void Update(int key, int action) {}
    ImGuiContext *GetContext() { return context_; }
    bool WantCaptureKeyboard() { return true; }
    ~KSubscription() {}

  private:
    ImGuiContext *context_;
  };

  class SSubscription : public Events::ScrollwheelSubscriber {
  public:
    SSubscription(ImGuiContext *context) { context_ = context; }
    void Update(double yoffset) {};
    ImGuiContext *GetContext() { return context_; }
    bool WantCaptureScroll() { return true; }
    ~SSubscription() {}

  private:
    ImGuiContext *context_;
  };

  class MSubscription : public Events::MouseSubscriber {
  public:
    MSubscription(ImGuiContext *context) { context_ = context; }
    void Update(double xpos, double ypos) {};
    bool WantCaptureMouse() { return true; }
    ImGuiContext *GetContext() { return context_; }
    void ResetCentre() {}
    ~MSubscription() {}

  private:
    ImGuiContext *context_;
    bool click_state_ = false;
  };

  std::shared_ptr<KSubscription> k_subscription_;
  std::shared_ptr<SSubscription> s_subscription_;
  std::shared_ptr<MSubscription> m_subscription_;

private:
  std::shared_ptr<GraphPanel> graph_panel_;
  GLFWwindow *window_;
  ImGuiContext *prev_context_;
  ImGuiContext *context_;
  ImGuiIO io;
  std::shared_ptr<Events::Controller> io_ctr_;
};

} // namespace Windowing

#endif // !FDVS_SRC_PANEL_HPP_

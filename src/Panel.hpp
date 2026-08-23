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
#include <cmath>
#include <dlfcn.h>

#include "Buffers.hpp"
#include "Data.hpp"
#include "Events.hpp"
#include "Graphics.hpp"
#include "MatrixHandler.hpp"
#include "Models.hpp"
#include "ShaderHandler.hpp"
#include "Strategy.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

#include "Themes.hpp"

#define SO_PATH "./libfunc.so"

typedef std::array<double, 2> Point2D;

namespace Windowing {

class Panel {
public:
  virtual void Render() = 0;
  virtual ~Panel() {}

protected:
  ImGuiContext *context;
};

class GraphPanel : public Panel {
public:
  GraphPanel(std::shared_ptr<Shader> in_shader,
             std::shared_ptr<Events::Controller> io_ctr);
  void Render();
  void SetModel(std::shared_ptr<Data::Bounds2D> b,
                std::function<float(float, float)> f, double z_scaling) {
    bounds = b;
    domain_R2_function_ = f;
    z_scaling_ = z_scaling;
  }
  void ReloadModel() {
    if (bounds != nullptr) {
      data_ = std::make_shared<Data::OffMeshData>(bounds, domain_R2_function_);
      data_->ScaleZ(z_scaling_);
      data_->ExportToOff("output_scaled");
      std::cerr << z_scaling_ << std::endl;
      graph->updateData(data_);
    } else {
      throw Data::INVALID_BOUNDS_EXCEPTION{};
    }
  }
  ~GraphPanel() {
    if(so_handle != nullptr)
      dlclose(so_handle);
  }

  std::shared_ptr<Data::OffMeshData> data_;

private:
  float depth;
  double z_scaling_;
  void *so_handle = nullptr;

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
  ConfigPanel(std::shared_ptr<Events::Controller> io_ctr,
              std::shared_ptr<GraphPanel> graph_panel);
  void Render();
  ~ConfigPanel() {}

  class KSubscription : public Events::KeyboardSubscriber {
  public:
    KSubscription() = default; 
    void Update(int key, int action) {}
    ImGuiWindow *GetWindow() {return nullptr;}
    bool WantCaptureKeyboard() { return true; }
    ~KSubscription() {}

  private:
    ImGuiContext *context_;
  };

  class SSubscription : public Events::ScrollwheelSubscriber {
  public:
    SSubscription() = default; 
    void Update(double yoffset) {};
    ImGuiWindow *GetWindow() {return nullptr;}
    bool WantCaptureScroll() { return true; }
    ~SSubscription() {}

  private:
    ImGuiContext *context_;
  };

  class MSubscription : public Events::MouseSubscriber {
  public:
    MSubscription() = default; 
    void Update(double xpos, double ypos) {};
    bool WantCaptureMouse() { return true; }
    ImGuiWindow *GetWindow() {return nullptr;}
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

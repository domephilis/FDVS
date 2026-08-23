#ifndef FDVS_SRC_MATRIX_HANDLER_HPP_
#define FDVS_SRC_MATRIX_HANDLER_HPP_

#include <iostream>
#include <memory>
#include <stack>
#include <string>
// GLM #include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "Events.hpp"
#include "ShaderHandler.hpp"

#ifndef GL_HEADERS_
#define GL_HEADERS_
// Glad First
#include <glad/glad.h>

// GLFW3 Second
#include <GLFW/glfw3.h>
#endif // !GLAD_H_

#include "imgui.h"
#include "imgui_internal.h"

namespace Matrices {

class RefFrame {
public:
  // Note: All vectors are expected to be normalized,
  // use the normalize function
  RefFrame() {
    loc_ = glm::vec3(0.0f, 0.0f, 0.0f);
    up_ = glm::vec3(0.0f, 1.0f, 0.0f);
    forward_ = glm::vec3(0.0f, 0.0f, -1.0f);
    lateral_ = glm::cross(forward_, up_);
  }
  RefFrame(glm::vec3 loc, glm::vec3 up, glm::vec3 forward);
  // Rotation Axis Vector has loc_, up_, forward_ as bases
  void RotateAxis(float angle_rad, glm::vec3 axis);
  void TranslateLoc(float amt, glm::vec3 translation_direction);

  glm::mat4 GetTransformMatrix();

  glm::vec3 loc_;
  glm::vec3 up_;
  glm::vec3 forward_;
  glm::vec3 lateral_;
};

// Todo: Create a ModelFrame class here

// We specialize here because the subscriber behavior is different
class CameraFrame : public RefFrame {
public:
  CameraFrame();
  CameraFrame(glm::vec3 loc, glm::vec3 up, glm::vec3 forward);
  void SetWindow(ImGuiWindow* window) {window_ = window;}

  class CFrameMSubscriber : public Events::MouseSubscriber {
  public:
    CFrameMSubscriber(Matrices::CameraFrame *camera) { camera_ = camera; }
    void UpdateMouseButtonState(bool click_state) override
    { 
      // std::cerr << "old: " << click_state_ << '\n'
      //   << "new: " << click_state << std::endl;
      if(click_state_ == false && click_state == true)
      {
        first_mouse_ = true;
        std::cerr << first_mouse_ << std::endl;
      }

      click_state_ = click_state;
    }

    glm::vec3 ArcballPoint(double x, double y)
    {
      GLint viewport[4];
      glGetIntegerv(GL_VIEWPORT, viewport);
      auto width = viewport[2];
      auto height = viewport[3];

      // viewport coordinates -> [-1, 1]
      double nx = (2.0 * x - width) / width;
      double ny = (height - 2.0 * y) / height;

      double r2 = nx * nx + ny * ny;

      if (r2 <= 1.0) {
          return glm::normalize(glm::vec3(
              nx,
              ny,
              std::sqrt(1.0 - r2)
          ));
      }

      // Outside sphere: project onto rim
      return glm::normalize(glm::vec3(nx, ny, 0.0));
    }

    void Update(double x_pos, double y_pos) override {

      // // std::cerr << "Mouse Callback Invoked" << std::endl;
      if(first_mouse_)
      {
        last_x_ = x_pos;
        last_y_ = y_pos;
        first_mouse_ = false;
      }

      // double mouse_speed_ = 0.008f;
      // double delta_displacement_x = (x_pos - centre_x) - last_displacement_x;
      // double delta_displacement_y = (y_pos - centre_y) - last_displacement_y;
      // last_displacement_x = x_pos - centre_x;
      // last_displacement_y = y_pos - centre_y;
      // // last_time_ = glfwGetTime();

      // camera_->RotateAxis(mouse_speed_ * delta_displacement_x,
      //                     glm::vec3(0.0f, -1.0f, 0.0f));
      // camera_->RotateAxis(mouse_speed_ * delta_displacement_y,
      //                     glm::vec3(1.0f, 0.0f, 0.0f));
     
      glm::vec3 old_p = ArcballPoint(last_x_, last_y_);
      glm::vec3 new_p = ArcballPoint(x_pos, y_pos);

      glm::vec3 axis = glm::cross(old_p, new_p);

      double d = glm::clamp(
          static_cast<double>(glm::dot(old_p, new_p)),
          -1.0,
          1.0
      );

      double angle = std::acos(d);

      if (glm::length(axis) > 1e-6f)
          camera_->RotateAxis(angle, glm::normalize(axis));

      last_x_ = x_pos;
      last_y_ = y_pos;
    }

    void UpdateWindowCentre(float x, float y) {
      centre_x = x;
      centre_y = y;
    }
    void ResetCentre() override {
      last_displacement_x = 0;
      last_displacement_y = 0;
    }
    ImGuiWindow *GetWindow() override { return camera_->window_; }
    bool WantCaptureMouse() override {
      return (IsLeftMouseButtonPressed() && (camera_->context_->NavWindow == camera_->window_) && user_flag_ &&
              camera_->io->WantCaptureMouse);
    }
    void SetUserFlag(bool val) { user_flag_ = val; }

    ~CFrameMSubscriber() {}

    bool first_mouse_ = true;
  private:
    CameraFrame *camera_;
    bool user_flag_ = true;
    double last_displacement_x = 0;
    double last_displacement_y = 0;
    double last_x_ = 0;
    double last_y_ = 0;
    float centre_x;
    float centre_y;
  };

  class CFrameSSubscriber : public Events::ScrollwheelSubscriber {
  public:
    CFrameSSubscriber(Matrices::CameraFrame *camera) { camera_ = camera; }
    void Update(double yoffset) override {
      camera_->TranslateLoc(yoffset * 20.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    }
    void SetUserFlag(bool val) { user_flag_ = val; }
    bool WantCaptureScroll() override { return user_flag_ && (camera_->context_->NavWindow == camera_->window_); }
    ImGuiWindow *GetWindow() override { return camera_->window_; }

  private:
    bool user_flag_ = true;
    CameraFrame *camera_;
  };

  class CFrameKSubscriber : public Events::KeyboardSubscriber {
  public:
    CFrameKSubscriber(Matrices::CameraFrame *camera) { camera_ = camera; }
    bool WantCaptureKeyboard() override {
      return (camera_->context_->NavWindow == camera_->window_);
    }
    void Update(int key, int action) override {
      if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_J:
          camera_->TranslateLoc(5.0f, glm::vec3(0.0f, 1.0f, 0.0f));
          break;
        case GLFW_KEY_K:
          camera_->TranslateLoc(5.0f, glm::vec3(0.0f, -1.0f, 0.0f));
          break;
        case GLFW_KEY_H:
          camera_->TranslateLoc(5.0f, glm::vec3(1.0f, 0.0f, 0.0f));
          break;
        case GLFW_KEY_L:
          camera_->TranslateLoc(5.0f, glm::vec3(-1.0f, 0.0f, 0.0f));
          break;
        case GLFW_KEY_I:
          camera_->TranslateLoc(5.0f, glm::vec3(0.0f, 0.0f, -1.0f));
          break;
        case GLFW_KEY_M:
          camera_->TranslateLoc(5.0f, glm::vec3(0.0f, 0.0f, 1.0f));
          break;
        case GLFW_KEY_N:
          camera_->RotateAxis(0.1f, glm::vec3(0.0f, 0.0f, 1.0f));
          break;
        case GLFW_KEY_B:
          camera_->RotateAxis(0.1f, glm::vec3(0.0f, 0.0f, -1.0f));
          break;
        case GLFW_KEY_A:
          camera_->RotateAxis(0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
          break;
        case GLFW_KEY_D:
          camera_->RotateAxis(0.1f, glm::vec3(0.0f, -1.0f, 0.0f));
          break;
        case GLFW_KEY_W:
          camera_->RotateAxis(0.1f, glm::vec3(1.0f, 0.0f, 0.0f));
          break;
        case GLFW_KEY_S:
          camera_->RotateAxis(0.1f, glm::vec3(-1.0f, 0.0f, 0.0f));
          break;
        }
      }
      std::cerr << camera_->loc_[0] << " " << camera_->loc_[1] << " "
                << camera_->loc_[2] << std::endl;
    }
    ImGuiWindow *GetWindow() override { return nullptr; }
    void SetUserFlag(bool val) { user_flag_ = val; }
    ~CFrameKSubscriber() {}

  private:
    CameraFrame *camera_;
    bool user_flag_ = true;
  };

  std::shared_ptr<CFrameKSubscriber> k_subscription_;
  std::shared_ptr<CFrameSSubscriber> s_subscription_;
  std::shared_ptr<CFrameMSubscriber> m_subscription_;
  ImGuiContext *context_;
  ImGuiWindow *window_;
  ImGuiIO *io;
};

class MatrixStack {
public:
  MatrixStack(std::string name, std::shared_ptr<Shader> shader)
      : name_(name), shader_(shader) {}
  void pushMatrix(glm::mat4 to_push) { m_stack_.push(to_push); }
  void pushProduct(glm::mat4 to_push) {
    m_stack_.push(to_push * m_stack_.top());
  }
  glm::mat4 popMatrix() {
    glm::mat4 temp = m_stack_.top();
    m_stack_.pop();
    shader_->setMatrix(name_, temp);
    return temp;
  }

private:
  std::string name_;
  std::stack<glm::mat4> m_stack_;
  std::shared_ptr<Shader> shader_;
};

} // namespace Matrices

#endif // !FDVS_SRC_MATRIX_HANDLER_HPP_

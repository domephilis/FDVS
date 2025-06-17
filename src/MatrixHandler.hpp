#ifndef FDVS_SRC_MATRIX_HANDLER_HPP_
#define FDVS_SRC_MATRIX_HANDLER_HPP_

#include <memory>
#include <stack>
#include <string>

// GLM
#include <glm/glm.hpp>
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

private:
  glm::vec3 loc_;
  glm::vec3 up_;
  glm::vec3 forward_;
  glm::vec3 lateral_;
};

class CameraFrame : public RefFrame {
public:
  CameraFrame();
  CameraFrame(glm::vec3 loc, glm::vec3 up, glm::vec3 forward);

  class CFrameMSubscriber : public Events::MouseSubscriber {
  public:
    CFrameMSubscriber(Matrices::CameraFrame *camera) { camera_ = camera; }
    void Update(double x_pos, double y_pos) override {
      double mouse_speed_ = 0.001f;
      double delta_displacement_x = (x_pos - centre_x) - last_displacement_x;
      double delta_displacement_y = (y_pos - centre_y) - last_displacement_y;
      last_displacement_x = x_pos - centre_x;
      last_displacement_y = y_pos - centre_y;
      // last_time_ = glfwGetTime();

      camera_->RotateAxis(mouse_speed_ * delta_displacement_x,
                          glm::vec3(0.0f, 1.0f, 0.0f));
      camera_->RotateAxis(mouse_speed_ * delta_displacement_y,
                          glm::vec3(-1.0f, 0.0f, 0.0f));
    }

    void UpdateWindowCentre(float x, float y) {
      centre_x = x;
      centre_y = y;
    }

    ~CFrameMSubscriber() {}

  private:
    CameraFrame *camera_;
    double last_displacement_x = 0;
    double last_displacement_y = 0;
    float centre_x;
    float centre_y;
  };

  class CFrameKSubscriber : public Events::KeyboardSubscriber {
  public:
    CFrameKSubscriber(Matrices::CameraFrame *camera) { camera_ = camera; }
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
    }
    ~CFrameKSubscriber() {}

  private:
    CameraFrame *camera_;
    bool is_keyboard_input_allowed_ = true;
  };

  std::shared_ptr<CFrameKSubscriber> k_subscription_;
  std::shared_ptr<CFrameMSubscriber> m_subscription_;
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

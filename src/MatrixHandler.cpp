#include "MatrixHandler.hpp"

Matrices::RefFrame::RefFrame(glm::vec3 loc, glm::vec3 up, glm::vec3 forward)
    : loc_(loc), up_(up), forward_(forward) {
  lateral_ = glm::cross(forward_, up);
}

void Matrices::RefFrame::RotateAxis(float angle_rad, glm::vec3 axis) {
  up_ = glm::rotate(up_, angle_rad, axis);
  forward_ = glm::rotate(forward_, angle_rad, axis);
  lateral_ = glm::rotate(lateral_, angle_rad, axis);
}

void Matrices::RefFrame::TranslateLoc(float amt,
                                      glm::vec3 translation_direction) {
  glm::vec4 temp =
      glm::translate(glm::mat4(1.0f), amt * translation_direction) *
      glm::vec4(loc_, 1.0f);
  loc_ = glm::vec3(temp[0], temp[1], temp[2]);
}

glm::mat4 Matrices::RefFrame::GetTransformMatrix() {
  glm::mat4 matrix(glm::vec4(lateral_, 0.0f), glm::vec4(up_, 0.0f),
                   glm::vec4(forward_, 0.0f), glm::vec4(loc_, 1.0f));
  return matrix;
}

Matrices::CameraFrame::CameraFrame() {
  k_subscription_ = std::make_shared<CFrameKSubscriber>(this);
  m_subscription_ = std::make_shared<CFrameMSubscriber>(this);
}
Matrices::CameraFrame::CameraFrame(glm::vec3 loc, glm::vec3 up,
                                   glm::vec3 forward)
    : RefFrame(loc, up, forward) {
  k_subscription_ = std::make_shared<CFrameKSubscriber>(this);
  m_subscription_ = std::make_shared<CFrameMSubscriber>(this);
}

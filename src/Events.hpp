#ifndef FDVS_SRC_EVENTS_HPP_
#define FDVS_SRC_EVENTS_HPP_

#ifndef GL_HEADERS_
#define GL_HEADERS_

// GLAD First

#include <glad/glad.h>

// GLFW3 Second

#include <GLFW/glfw3.h>

#endif

#include <algorithm>
#include <memory>
#include <vector>

namespace Events {

class KeyboardSubscriber {
public:
  virtual void Update(int key, int action) = 0;
  virtual bool IsUpdateAllowed() = 0;
  virtual ~KeyboardSubscriber() {}
};

class MouseSubscriber {
public:
  virtual void Update(double xpos, double ypos) = 0;
  virtual bool IsUpdateAllowed() = 0;
  virtual ~MouseSubscriber() {}
  void UpdateClickState(bool val) { click_state_ = val; }
  bool LeftClickState() { return click_state_; }

private:
  bool click_state_ = false;
};

class KeyboardPublisher {
public:
  KeyboardPublisher(GLFWwindow *window) : window_(window) {
    glfwSetKeyCallback(window_, KeyCallback);
  }
  void addSubscriber(std::shared_ptr<KeyboardSubscriber> s) {
    subscribers_.push_back(s);
  }
  void removeSubscriber(std::shared_ptr<KeyboardSubscriber> s) {
    subscribers_.erase(std::remove(subscribers_.begin(), subscribers_.end(), s),
                       subscribers_.end());
  }
  static void KeyCallback(GLFWwindow *window, int key, int scancode, int action,
                          int mods);

  GLFWwindow *window_;
  std::vector<std::shared_ptr<KeyboardSubscriber>> subscribers_;
};

class MousePublisher {
public:
  MousePublisher(GLFWwindow *window) : window_(window) {
    glfwSetCursorPosCallback(window_, CursorPositionCallback);
  }
  void addSubscriber(std::shared_ptr<MouseSubscriber> s) {
    subscribers_.push_back(s);
  }
  void removeSubscriber(std::shared_ptr<MouseSubscriber> s) {
    subscribers_.erase(std::remove(subscribers_.begin(), subscribers_.end(), s),
                       subscribers_.end());
  }
  static void CursorPositionCallback(GLFWwindow *window, double xpos,
                                     double ypos);

  GLFWwindow *window_;
  std::vector<std::shared_ptr<MouseSubscriber>> subscribers_;
};

class Controller {
public:
  Controller(GLFWwindow *window);
  ~Controller() {}

  GLFWwindow *window_;
  std::unique_ptr<KeyboardPublisher> k_publisher_;
  std::unique_ptr<MousePublisher> m_publisher_;
};

} // namespace Events

#endif // !FDVS_SRC_EVENTS_HPP_

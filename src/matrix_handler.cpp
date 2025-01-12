#include "matrix_handler.hpp"

glm::mat4 PerspectiveFrustrum(float r, float l, float t, float b, float n,
                              float f) {
  glm::mat4 matrix((2 * n) / (r - l), 0.0f, (r + l) / (r - l), 0.0f, // 1st row
                   0.0f, (2 * n) / (t - b), (t + b) / (t - b), 0.0f, // 2nd row
                   0.0f, 0.0f, -(f + n) / (f - n),
                   -(2 * f * n) / (f - n), // 3rd row
                   0.0f, 0.0f, -1.0f, 0.0f // 4throw
  );
  return matrix;
}

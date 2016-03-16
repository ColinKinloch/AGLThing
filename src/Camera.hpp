#pragma once

#include "Node.hpp"

class Camera: public Node {
public:
  glm::mat4 projection;

  virtual glm::mat4 getMatrix() {
    glm::mat4 modelViewMatrix = Node::getMatrix();
    return projection * modelViewMatrix;
  }
};

#pragma once

#include <glm/vec3.hpp>
//#include <glm/mat4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <glm/gtc/matrix_transform.hpp>

class Node {
public:
  Node() {
    scaling = glm::vec3(1);
  }

  glm::vec3 position;
  glm::quat orientation;
  //glm::vec3 rotation;
  glm::vec3 scaling;

  virtual glm::mat4 getMatrix() {
    glm::mat4 modelViewMatrix = glm::mat4_cast(orientation);
    modelViewMatrix = glm::translate(modelViewMatrix, position);
    modelViewMatrix = glm::scale(modelViewMatrix, scaling);
    return modelViewMatrix;
  }
};

#pragma once

#include <thread>

#include <gtkmm/glarea.h>
#include <gtkmm/builder.h>

#include <epoxy/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Vertex {
  glm::vec4 position;
  glm::vec3 normal;
  glm::vec4 colour;
};
struct Attribute {
  GLint id;
  std::string name;
};
struct Uniform {
  GLint id;
  std::string name;
};

class Renderer: public Gtk::GLArea {
public:
  Renderer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder);
  Renderer();

  static GLuint createShader(GLenum type, std::string uri);
  static GLuint createProgram(std::vector<GLuint> shaders);

  static void debug_callback(GLenum source, GLenum type, GLuint id,
  GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

  glm::quat orientation;
  glm::vec3 rotation;
  glm::vec3 position;

protected:
  GLuint program;
  GLuint* vao;
  GLuint* buffers;
  GLuint vertexShader;
  GLuint fragmentShader;

  std::map<std::string, Attribute> attributes;
  std::map<std::string, Uniform> uniforms;

  std::vector<unsigned char> indices;
  std::vector<Vertex> vertices;

  std::chrono::system_clock::time_point startTime;
  std::chrono::system_clock::time_point previousTime;

  std::thread renderThread;

  glm::mat3 normalMatrix;
  glm::mat4 projectionMatrix;

  glm::mat4 modelViewMatrix;
  glm::mat4 modelViewProjectionMatrix;


  bool on_render(const Glib::RefPtr< Gdk::GLContext >&);
  void on_resize(int width, int height);

  Glib::RefPtr< Gdk::GLContext > on_create_context();

  void on_realize();
  void on_unrealize();

};
